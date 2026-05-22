#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../utils/Types.hpp"

namespace endeus {

	struct LayerData {
		std::string textureKey;
		int order = 0;
		bool visible = true;

		Vec2f position{ 0, 0 };
		float alpha = 1.0f;
		Rect texRect{ 0,0,0,0 };

		mutable bool dirty = true;

		bool operator==(const LayerData& other) const {
			return this->textureKey == other.textureKey
				&& this->order == other.order
				&& this->position == other.position
				&& this->alpha == other.alpha
				&& this->visible == other.visible
				&& this->texRect == other.texRect;
		}

		bool operator!=(const LayerData& other) const {
			return !(*this == other);
		}
	};

	struct ChoiceOption {
		std::string text;
		std::string targetLabel;
	};

	class WorldModel {
	public:
		void setLayer(const std::string& id, const LayerData& data);
		void hideLayer(const std::string& id);
		//void removeLayer(const std::string& id);
		const LayerData* getLayer(const std::string& id) const;
		const std::unordered_map<std::string, LayerData>& allLayers() const;

		void setSpeaker(const std::string& name);
		void setContent(const std::string& text, bool append);
		std::string getSpeaker() const;
		std::string getContent() const;

		void setChoice(const std::vector<ChoiceOption>& options);
		void clearChoice();
		bool hasChoice() const;
		const std::vector<ChoiceOption>& getChoiceOptions() const;

	private:
		std::unordered_map<std::string, LayerData> m_layers;		// layerId -> LayerData
		std::string m_speaker;
		std::string m_content;
		std::vector<ChoiceOption> m_choiceOptions;
	};

} // namespace endeus