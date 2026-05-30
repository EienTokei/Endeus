#include "Renderer.hpp"
#include "../anemoi/MoveLayerAnemos.hpp"
#include "../anemoi/FadeLayerAnemos.hpp"

#include <iostream>

namespace endeus {

	Renderer::Renderer(Leyline& leyline, sf::RenderWindow& window, Anemoi& anemoi)
		: m_leyline(leyline), m_window(window), m_anemoi(anemoi), m_speakerText(nullptr), m_contentText(nullptr) {
		// 订阅点击事件
		m_leyline.subscribe<Event::MouseClicked>([this](const Event& e) {
			if (auto* ev = e.getIf<Event::MouseClicked>()) {
				sf::Vector2i pos(static_cast<int>(ev->position.x), static_cast<int>(ev->position.y));
				onMouseClick(pos);
			}
		});
	}

	void Renderer::registerTexture(const std::string& key, sf::Texture&& texture) {
		m_textures[key] = std::make_unique<sf::Texture>(std::move(texture));
	}

	void Renderer::setFont(const sf::Font& font) {
		m_font = font;
		m_speakerText = std::make_unique<sf::Text>(m_font);
		m_speakerText->setFillColor(sf::Color::Yellow);
		m_speakerText->setCharacterSize(30);

		m_contentText = std::make_unique<sf::Text>(m_font);
		m_contentText->setFillColor(sf::Color::White);
		m_contentText->setCharacterSize(24);
	}

	void Renderer::onMouseClick(sf::Vector2i pos) {
		// 优先级：选项 > 等待点击
		if (!m_options.empty()) {
			sf::Vector2f fpos(static_cast<float>(pos.x), static_cast<float>(pos.y));
			const auto& optionRects = computeOptionRects();
			for (size_t i = 0; i < optionRects.size(); ++i) {
				if (optionRects[i].contains(fpos)) {
					// 选中选项
					std::string target = m_options[i].targetLabel;
					m_options.clear();
					m_leyline.publish(Event::ChoiceSelected{ target });	// 发布选中事件
					return;
				}
			}
		}
		else {
			m_leyline.publish(Event::ActionCompleted{});
		}
	}

	void Renderer::draw(const World& world, std::vector<Instruction::Choice::Option> options) {
		if (!options.empty()) {
			m_options = options;
		}
		syncDirtyLayers(world);
		applyAnemoiOverrides();
		// 1. 图层
		drawLayers(world);
		// 2. 对话框
		drawDialog(world);
		// 3. 选项
		drawChoices();
	}

	void Renderer::syncDirtyLayers(const World& world) {
		for (auto& [id, data] : world.layers) {
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

	void Renderer::applyAnemoiOverrides() {
		for (auto& [key, anemos] : m_anemoi.getAnemosMap()) {
			sf::Sprite* sprite = m_layers[key.layerId].get();
			if (key.property == AnemosKey::Property::Alpha) {
				if (auto* fade = dynamic_cast<const FadeLayerAnemos*>(anemos.get())) {
					sf::Color color = sprite->getColor();
					color.a = static_cast<unsigned char>(fade->getCurrent() * 255);
					sprite->setColor(color);
				}
			}
			if (key.property == AnemosKey::Property::Position) {
				if (auto* move = dynamic_cast<const MoveLayerAnemos*>(anemos.get())) {
					Vec2f pos = move->getCurrent();
					sprite->setPosition({ pos.x, pos.y });
				}
			}
		}
	}

	void Renderer::drawLayers(const World& world) const {
		std::vector<std::pair<int, const sf::Sprite*>> sorted;
		for (const auto& [id, data] : world.layers) {
			sf::Sprite* sprite = m_layers.at(id).get();
			if (data.visible && sprite) {
				sorted.emplace_back(data.order, sprite);
			}
		}
		std::sort(sorted.begin(), sorted.end(), [](auto& a, auto& b) { return a.first < b.first; });

		for (auto& [order, sprite] : sorted) {
			m_window.draw(*sprite);
		}
	}

	void Renderer::drawDialog(const World& world) const {
		m_speakerText->setString(toSF(world.speaker));
		m_contentText->setString(toSF(world.content));

		float w = static_cast<float>(m_window.getSize().x);
		float h = static_cast<float>(m_window.getSize().y);

		sf::RectangleShape dialogBox;
		// 左右各50像素, 底部30像素
		dialogBox.setSize({ w - 100, 150 });
		dialogBox.setFillColor(sf::Color(0, 0, 0, 200));
		dialogBox.setPosition({ 50, h - 180 });
		m_window.draw(dialogBox);

		m_speakerText->setPosition({ 70, h - 170 });
		m_window.draw(*m_speakerText);

		m_contentText->setPosition({ 70, h - 120 });
		m_window.draw(*m_contentText);
	}

	std::vector<sf::FloatRect> Renderer::computeOptionRects() const {
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

	void Renderer::drawChoices() const {
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
			m_window.draw(box);

			sf::Text text(m_font);
			text.setString(toSF(m_options[i].text));
			text.setCharacterSize(24);
			text.setFillColor(sf::Color::White);
			text.setPosition({ rect.position.x + 10, rect.position.y + 10 });
			m_window.draw(text);
		}
	}

} // namespace endeus
