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

		auto& layer = m_layers[instr.layerId];
		if (!layer.sprite) {
			layer.sprite = std::make_unique<sf::Sprite>(*texIt->second);
		}
		else {
			layer.sprite->setTexture(*texIt->second);
		}

		layer.sprite->setPosition({ instr.position.x, instr.position.y });
		if (instr.texRect.width > 0 && instr.texRect.height > 0) {
			layer.sprite->setTextureRect(sf::IntRect(
				{ instr.texRect.x, instr.texRect.y },
				{ instr.texRect.width, instr.texRect.height }
			));
		}
		layer.order = instr.order;
		layer.visible = true;
	}

	void Runtime::handleHideLayer(const Instruction::HideLayer& instr) {
		auto it = m_layers.find(instr.layerId);
		if (it != m_layers.end()) {
			it->second.visible = false;
		}
	}

	void Runtime::handleSetSpeaker(const Instruction::SetSpeaker& instr) {
		m_speaker = instr.name;
		m_speakerText->setString(toSF(m_speaker));
	}

	void Runtime::handleSetContent(const Instruction::SetContent& instr) {
		if (instr.append) {
			m_content += instr.content;
		}
		else {
			m_content = instr.content;
		}
		m_contentText->setString(toSF(m_content));
	}

	void Runtime::handleChoice(const Instruction::Choice& instr) {
		m_choiceState.active = true;
		m_choiceState.options = instr.options;

		float optionW = 300;
		float optionH = 50;
		float spaceY = 30;

		int n = instr.options.size();
		float startX = (static_cast<float>(m_window.getSize().x) - optionW) / 2;
		float startY = (static_cast<float>(m_window.getSize().y) - optionH * n - spaceY * (n - 1)) / 2;

		m_choiceState.optionRects.clear();
		for (size_t i = 0; i < instr.options.size(); ++i) {
			sf::FloatRect rect({ startX, startY + i * (optionH + 30) }, 
							   { optionW, optionH });
			m_choiceState.optionRects.emplace_back(rect);
		}
	}

	void Runtime::handleWaitForClick() {
		m_waitingForClick = true;
	}

	void Runtime::onMouseClick(sf::Vector2i pos) {
		// 优先级：选项 > 等待点击
		if (m_choiceState.active) {
			sf::Vector2f fpos(static_cast<float>(pos.x), static_cast<float>(pos.y));
			for (size_t i = 0; i < m_choiceState.optionRects.size(); ++i) {
				if (m_choiceState.optionRects[i].contains(fpos)) {
					// 选中选项
					std::string target = m_choiceState.options[i].targetLabel;
					m_choiceState.active = false;
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

	void Runtime::draw(sf::RenderTarget& target) const {
		// 1. 按 order 从小到大绘制图层
		std::vector<std::pair<int, const Layer*>> sorted;
		for (const auto& pair : m_layers) {
			if (pair.second.visible && pair.second.sprite)
				sorted.emplace_back(pair.second.order, &pair.second);
		}
		std::sort(sorted.begin(), sorted.end(),
				  [](auto& a, auto& b) { return a.first < b.first; });
		for (auto& [order, layer] : sorted) {
			target.draw(*layer->sprite);
		}

		// 2. 对话框
		drawDialog(target);
		// 3. 选项
		drawChoices(target);
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

	void Runtime::drawChoices(sf::RenderTarget& target) const {
		if (!m_choiceState.active) {
			return;
		}

		for (size_t i = 0; i < m_choiceState.options.size(); ++i) {
			const auto& rect = m_choiceState.optionRects[i];
			sf::RectangleShape box;
			box.setSize(rect.size);
			box.setPosition(rect.position);
			box.setFillColor(sf::Color(40, 40, 40, 220));
			box.setOutlineColor(sf::Color::White);
			box.setOutlineThickness(2);
			target.draw(box);

			sf::Text text(m_font);
			text.setString(toSF(m_choiceState.options[i].text));
			text.setCharacterSize(24);
			text.setFillColor(sf::Color::White);
			text.setPosition({ rect.position.x + 10, rect.position.y + 10 });
			target.draw(text);
		}
	}
}
