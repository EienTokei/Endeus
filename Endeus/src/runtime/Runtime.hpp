#pragma once

#include "IRuntime.hpp"
#include "../event/EventBus.hpp"
#include "WorldModel.hpp"

#include <unordered_map>
#include <SFML/Graphics.hpp>

namespace endeus {

	class Runtime : public IRuntime {
	public:
		Runtime(EventBus& eventBus, sf::RenderWindow& window);

		void registerTexture(const std::string& key, sf::Texture&& texture);
		void setFont(const sf::Font& font);

		bool execute(const Instruction& instr) override;
		void update(float dt) override;

		void draw(sf::RenderTarget& target);

	private:
		void onMouseClick(sf::Vector2i pos);

		void handleShowLayer(const Instruction::ShowLayer& instr);
		void handleHideLayer(const Instruction::HideLayer& instr);
		void handleSetSpeaker(const Instruction::SetSpeaker& instr);
		void handleSetContent(const Instruction::SetContent& instr);
		void handleChoice(const Instruction::Choice& instr);
		void handleWaitForClick();

		void syncDirtyLayers();
		void drawLayers(sf::RenderTarget& target) const;
		void drawDialog(sf::RenderTarget& target) const;
		std::vector<sf::FloatRect> computeOptionRects() const;
		void drawChoices(sf::RenderTarget& target) const;

	private:

		WorldModel m_world;
		EventBus& m_eventBus;
		sf::RenderWindow& m_window;
		std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;	// textureKey -> Texture
		std::unordered_map<std::string, std::unique_ptr<sf::Sprite>> m_layers;		// layerId -> Sprite
		sf::Font m_font;
		
		mutable std::unique_ptr<sf::Text> m_speakerText;
		mutable std::unique_ptr<sf::Text> m_contentText;

		bool m_waitingForClick = false;
	};

	inline sf::String toSF(const std::string str) {
		return sf::String::fromUtf8(str.begin(), str.end());
	}

} // namespace endeus