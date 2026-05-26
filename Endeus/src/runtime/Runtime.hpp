#pragma once

#include "../script/IExecutor.hpp"
#include "../event/EventBus.hpp"
#include "WorldModel.hpp"
#include "anemoi/Anemoi.hpp"

#include <unordered_map>
#include <SFML/Graphics.hpp>

namespace endeus {

	class Runtime : public IExecutor {
	public:
		Runtime(EventBus& eventBus, sf::RenderWindow& window);

		void registerTexture(const std::string& key, sf::Texture&& texture);
		void setFont(const sf::Font& font);

		bool execute(const Instruction& instr) override;
		void update(float dt) override;
		//void resetAsync() override;

		void draw(sf::RenderTarget& target);

	private:
		void onMouseClick(sf::Vector2i pos);

		// true - 同步完成; false - 需要等待
		bool handleShowLayer(const Instruction::ShowLayer& instr);
		bool handleHideLayer(const Instruction::HideLayer& instr);
		bool handleMoveLayer(const Instruction::MoveLayer& instr);
		bool handleFadeLayer(const Instruction::FadeLayer& instr);
		bool handleSetSpeaker(const Instruction::SetSpeaker& instr);
		bool handleSetContent(const Instruction::SetContent& instr);
		bool handleChoice(const Instruction::Choice& instr);
		bool handleWaitForClick();

		void syncDirtyLayers();
		void drawLayers(sf::RenderTarget& target) const;
		void drawDialog(sf::RenderTarget& target) const;
		std::vector<sf::FloatRect> computeOptionRects() const;
		void drawChoices(sf::RenderTarget& target) const;

	private:

		WorldModel m_world;
		Anemoi m_anemoi;
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