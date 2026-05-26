#include "Runtime.hpp"
#include "anemoi/MoveLayerAnemos.hpp"
#include "anemoi/FadeLayerAnemos.hpp"

#include <iostream>

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
			return handleShowLayer(*show);
		}
		if (auto* hide = instr.getIf<Instruction::HideLayer>()) {
			return handleHideLayer(*hide);
		}
		if (auto* move = instr.getIf<Instruction::MoveLayer>()) {
			return handleMoveLayer(*move);	// 不阻塞 Director 调度, 继续推进剧情
		}
		if (auto* fade = instr.getIf<Instruction::FadeLayer>()) {
			return handleFadeLayer(*fade);	// 不阻塞 Director 调度, 继续推进剧情
		}
		if (auto* speaker = instr.getIf<Instruction::SetSpeaker>()) {
			return handleSetSpeaker(*speaker);
		}
		if (auto* content = instr.getIf<Instruction::SetContent>()) {
			return handleSetContent(*content);
		}
		if (auto* choice = instr.getIf<Instruction::Choice>()) {
			return handleChoice(*choice);
		}
		if (auto* wait = instr.getIf<Instruction::Wait>()) {
			return handleWaitForClick();
		}
		// 暂不实现
		return true;
	}

	void Runtime::update(float dt) {
		if (m_anemoi.isPlaying()) {
			m_anemoi.update(dt, m_world);
		}
	}

	void Runtime::resetAsync() {
		m_anemoi.resetAll(m_world);
		m_waitingForClick = false;
		m_options.clear();
	}

	bool Runtime::handleShowLayer(const Instruction::ShowLayer& instr) {
		auto texIt = m_textures.find(instr.textureKey);
		if (texIt == m_textures.end()) {
			std::cerr << "show: no texture - " << instr.textureKey << std::endl;
			return true;
		}
		LayerData data{ instr.textureKey, instr.order, true, instr.position, instr.alpha , instr.texRect };

		if (!m_world.addLayer(instr.layerId, data)) {
			m_world.setLayerData(instr.layerId, data);
		}
		return true;
	}

	bool Runtime::handleHideLayer(const Instruction::HideLayer& instr) {
		m_world.setLayerVisible(instr.layerId, false);
		return true;
	}

	bool Runtime::handleMoveLayer(const Instruction::MoveLayer& instr) {
		const LayerData* layer = m_world.getLayer(instr.layerId);
		if (!layer) {
			std::cerr << "move: no layer - " << instr.layerId << std::endl;
			return true;
		}

		auto anemos = std::make_unique<MoveLayerAnemos>(
			instr.layerId,
			instr.toPosition,
			instr.durationSeconds
		);
		m_anemoi.add(std::move(anemos));
		return true;
	}

	bool Runtime::handleFadeLayer(const Instruction::FadeLayer& instr) {
		const LayerData* layer = m_world.getLayer(instr.layerId);
		if (!layer) {
			std::cerr << "move: no layer - " << instr.layerId << std::endl;
			return true;
		}

		auto anemos = std::make_unique<FadeLayerAnemos>(
			instr.layerId,
			instr.toAlpha,
			instr.durationSeconds
		);
		m_anemoi.add(std::move(anemos));
		return true;
	}

	bool Runtime::handleSetSpeaker(const Instruction::SetSpeaker& instr) {
		m_world.setSpeaker(instr.name);
		m_speakerText->setString(toSF(m_world.getSpeaker()));
		return true;
	}

	bool Runtime::handleSetContent(const Instruction::SetContent& instr) {
		m_world.setContent(instr.content, instr.append);
		m_contentText->setString(toSF(m_world.getContent()));
		return true;
	}

	bool Runtime::handleChoice(const Instruction::Choice& instr) {
		m_options = instr.options;
		return false;
	}

	bool Runtime::handleWaitForClick() {
		m_waitingForClick = true;
		return false;
	}

	void Runtime::onMouseClick(sf::Vector2i pos) {
		// 优先级：选项 > 等待点击
		if (!m_options.empty()) {
			sf::Vector2f fpos(static_cast<float>(pos.x), static_cast<float>(pos.y));
			const auto& optionRects = computeOptionRects();
			for (size_t i = 0; i < optionRects.size(); ++i) {
				if (optionRects[i].contains(fpos)) {
					// 选中选项
					std::string target = m_options[i].targetLabel;
					m_options.clear();
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
			auto texIt = m_textures.find(data.textureKey);		// 1. 查找纹理
			if (texIt == m_textures.end()) {
				// 纹理不存在，跳过
				std::cerr << "sync: no texture - " << data.textureKey << std::endl;
				data.dirty = false;
				continue;
			}

			auto spriteIt = m_layers.find(id);					// 2. 是否存在缓存
			bool needCreate = (spriteIt == m_layers.end());
			if (needCreate) {
				// 创建新 Sprite
				auto sprite = std::make_unique<sf::Sprite>(*texIt->second);
				m_layers[id] = std::move(sprite);
			}

			if (needCreate || data.dirty) {						// 3. 设置属性
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

		int n = m_options.size();
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
		if (m_options.empty()) {
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
			text.setString(toSF(m_options[i].text));
			text.setCharacterSize(24);
			text.setFillColor(sf::Color::White);
			text.setPosition({ rect.position.x + 10, rect.position.y + 10 });
			target.draw(text);
		}
	}

} // namespace endeus