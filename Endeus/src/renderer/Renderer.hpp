#pragma once

#include "../event/EventBus.hpp"
#include "../world/World.hpp"
#include "../script/Instruction.hpp"
#include "../anemoi/Anemoi.hpp"

#include <unordered_map>
#include <SFML/Graphics.hpp>

namespace endeus {

	class Renderer {
	public:
		Renderer(EventBus& eventBus, sf::RenderWindow& window, Anemoi& anemoi);

		void registerTexture(const std::string& key, sf::Texture&& texture);
		void setFont(const sf::Font& font);

		void draw(const World& world, std::vector<Instruction::Choice::Option> options);

	private:
		void onMouseClick(sf::Vector2i pos);

		void syncDirtyLayers(const World& world);
		void applyAnemoiOverrides();

		void drawLayers(const World& world) const;
		void drawDialog(const World& world) const;
		std::vector<sf::FloatRect> computeOptionRects() const;
		void drawChoices() const;

	private:

		EventBus& m_eventBus;
		sf::RenderWindow& m_window;
		const Anemoi& m_anemoi;
		std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;	// textureKey -> Texture
		std::unordered_map<std::string, std::unique_ptr<sf::Sprite>> m_layers;		// layerId -> Sprite
		sf::Font m_font;

		mutable std::unique_ptr<sf::Text> m_speakerText;
		mutable std::unique_ptr<sf::Text> m_contentText;

		std::vector<Instruction::Choice::Option> m_options;
	};

	inline sf::String toSF(const std::string str) {
		return sf::String::fromUtf8(str.begin(), str.end());
	}

} // namespace endeus
