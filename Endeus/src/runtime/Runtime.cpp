#include "Runtime.hpp"

namespace endeus {

	Runtime::Runtime(EventBus& bus, sf::RenderWindow& window)
		: m_eventBus(bus), m_window(window)
		, m_speakerText(nullptr), m_contentText(nullptr){
		// 订阅点击事件
		m_eventBus.subscribe<Event::MouseClicked>([this](const Event& e) {
			if (auto* ev = e.getIf<Event::MouseClicked>()) {
				sf::Vector2i pos(static_cast<int>(ev->position.x), static_cast<int>(ev->position.y));
				onMouseClick(pos);
			}
		});
	}

	void Runtime::registerTexture(const std::string& key, sf::Texture&& texture) {
		m_textures[key] = std::make_unique<sf::Texture>(std::move(texture));
	}

	void Runtime::setFont(const sf::Font& font) {
		m_font = font;
		m_speakerText = std::make_unique<sf::Text>(m_font);
		m_speakerText->setFillColor(sf::Color::Yellow);
		m_speakerText->setCharacterSize(30);

		m_contentText = std::make_unique<sf::Text>(m_font);
		m_contentText->setFillColor(sf::Color::White);
		m_contentText->setCharacterSize(24);
	}

	bool Runtime::execute(const Instruction& instr) {
		if (auto* show = instr.getIf<Instruction::ShowLayer>()) {
			handleShowLayer(*show);
			return true;
		}
		if (auto* hide = instr.getIf<Instruction::HideLayer>()) {
			handleHideLayer(*hide);
			return true;
		}
		if (auto* speaker = instr.getIf<Instruction::SetSpeaker>()) {
			handleSetSpeaker(*speaker);
			return true;
		}
		if (auto* content = instr.getIf<Instruction::SetContent>()) {
			handleSetContent(*content);
			return true;
		}
		if (auto* choice = instr.getIf<Instruction::Choice>()) {
			handleChoice(*choice);
			return false;
		}
		if (auto* wait = instr.getIf<Instruction::Wait>()) {
			if (wait->seconds == 0.f) {
				handleWaitForClick();
				return false;
			}
			// 暂不实现
			return true;
		}
		// 暂不实现
		return true;
	}

	void Runtime::update(float dt) {
		// 暂不实现
	}

	void Runtime::handleShowLayer(const Instruction::ShowLayer& instr) {
		auto texIt = m_textures.find(instr.textureKey);
		if (texIt == m_textures.end()) {
			return;
		}

		m_world.setLayer(instr.layerId, { instr.textureKey, instr.order, true, instr.position, instr.alpha , instr.texRect});
	}

	void Runtime::handleHideLayer(const Instruction::HideLayer& instr) {
		m_world.hideLayer(instr.layerId);
	}

	void Runtime::handleSetSpeaker(const Instruction::SetSpeaker& instr) {
		m_world.setSpeaker(instr.name);
		m_speakerText->setString(toSF(m_world.getSpeaker()));
	}

	void Runtime::handleSetContent(const Instruction::SetContent& instr) {
		m_world.setContent(instr.content, instr.append);
		m_contentText->setString(toSF(m_world.getContent()));
	}

	void Runtime::handleChoice(const Instruction::Choice& instr) {
		std::vector<ChoiceOption> options;
		for (const auto& opt : instr.options) {
			options.emplace_back(ChoiceOption{opt.text, opt.targetLabel});
		}
		m_world.setChoice(std::move(options));
	}

	void Runtime::handleWaitForClick() {
		m_waitingForClick = true;
	}

	void Runtime::onMouseClick(sf::Vector2i pos) {
		// 优先级：选项 > 等待点击
		if (m_world.hasChoice()) {
			sf::Vector2f fpos(static_cast<float>(pos.x), static_cast<float>(pos.y));
			const auto& optionRects = computeOptionRects();
			for (size_t i = 0; i < optionRects.size(); ++i) {
				if (optionRects[i].contains(fpos)) {
					// 选中选项
					std::string target = m_world.getChoiceOptions()[i].targetLabel;
					m_world.clearChoice();
					m_eventBus.publish(Event::ChoiceSelected{ target });	// 发布选中事件
					return;
				}
			}
		}
		else if (m_waitingForClick) {
			m_waitingForClick = false;
			m_eventBus.publish(Event::ActionCompleted{});
		}
	}

	void Runtime::draw(sf::RenderTarget& target) {
		syncDirtyLayers();
		// 1. 图层
		drawLayers(target);
		// 2. 对话框
		drawDialog(target);
		// 3. 选项
		drawChoices(target);
	}

	void Runtime::syncDirtyLayers() {
		for (auto& [id, data] : m_world.allLayers()) {
			if (data.dirty) {
				auto texIt = m_textures.find(data.textureKey);
				if (texIt == m_textures.end()) {
					// 纹理不存在，跳过
					data.dirty = false;
					continue;
				}

				auto spriteIt = m_layers.find(id);
				if (spriteIt == m_layers.end()) {
					// 创建新 Sprite
					auto sprite = std::make_unique<sf::Sprite>(*texIt->second);
					m_layers[id] = std::move(sprite);
				}

				// 更新 Sprite 的属性
				sf::Sprite* sprite = m_layers[id].get();
				// 纹理改变
				if (&sprite->getTexture() != texIt->second.get()) {
					sprite->setTexture(*texIt->second, true);
				}

				sprite->setPosition({ data.position.x, data.position.y });

				sf::Color color = sf::Color::White;
				color.a = static_cast<unsigned char>(data.alpha * 255);
				sprite->setColor(color);

				if (data.texRect.width > 0 && data.texRect.height > 0) {
					sprite->setTextureRect(sf::IntRect(
						{ data.texRect.x, data.texRect.y },
						{ data.texRect.width, data.texRect.height }
					));
				}

				data.dirty = false;
			}
		}
	}

	void Runtime::drawLayers(sf::RenderTarget& target) const {
		std::vector<std::pair<int, const sf::Sprite*>> sorted;
		for (const auto& [id, data] : m_world.allLayers()) {
			sf::Sprite* sprite = m_layers.at(id).get();
			if (data.visible && sprite) {
				sorted.emplace_back(data.order, sprite);
			}
		}
		std::sort(sorted.begin(), sorted.end(), [](auto& a, auto& b) { return a.first < b.first; });

		for (auto& [order, sprite] : sorted) {
			target.draw(*sprite);
		}
	}

	void Runtime::drawDialog(sf::RenderTarget& target) const {
		float w = static_cast<float>(target.getSize().x);
		float h = static_cast<float>(target.getSize().y);

		sf::RectangleShape dialogBox;
		// 左右各50像素, 底部30像素
		dialogBox.setSize({ w - 100, 150 });
		dialogBox.setFillColor(sf::Color(0, 0, 0, 200));
		dialogBox.setPosition({ 50, h - 180 });
		target.draw(dialogBox);

		m_speakerText->setPosition({ 70, h - 170 });
		target.draw(*m_speakerText);

		m_contentText->setPosition({ 70, h - 120 });
		target.draw(*m_contentText);
	}

	std::vector<sf::FloatRect> Runtime::computeOptionRects() const {
		std::vector<sf::FloatRect> optionRects;

		float optionW = 300;
		float optionH = 50;
		float spaceY = 30;

		const auto& options = m_world.getChoiceOptions();
		int n = options.size();
		float startX = (static_cast<float>(m_window.getSize().x) - optionW) / 2;
		float startY = (static_cast<float>(m_window.getSize().y) - optionH * n - spaceY * (n - 1)) / 2;

		for (size_t i = 0; i < n; ++i) {
			sf::FloatRect rect({ startX, startY + i * (optionH + 30) },
							   { optionW, optionH });
			optionRects.emplace_back(rect);
		}
		return optionRects;
	}

	void Runtime::drawChoices(sf::RenderTarget& target) const {
		if (!m_world.hasChoice()) {
			return;
		}

		const auto& optionRects = computeOptionRects();

		for (size_t i = 0; i < optionRects.size(); ++i) {
			const auto& rect = optionRects[i];
			sf::RectangleShape box;
			box.setSize(rect.size);
			box.setPosition(rect.position);
			box.setFillColor(sf::Color(40, 40, 40, 220));
			box.setOutlineColor(sf::Color::White);
			box.setOutlineThickness(2);
			target.draw(box);

			sf::Text text(m_font);
			text.setString(toSF(m_world.getChoiceOptions()[i].text));
			text.setCharacterSize(24);
			text.setFillColor(sf::Color::White);
			text.setPosition({ rect.position.x + 10, rect.position.y + 10 });
			target.draw(text);
		}
	}

} // namespace endeus