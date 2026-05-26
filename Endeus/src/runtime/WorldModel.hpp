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

	class WorldModel {
	public:
		bool addLayer(const std::string& id, const LayerData& data);
		//void removeLayer(const std::string& id);
		const LayerData* getLayer(const std::string& id) const;
		const std::unordered_map<std::string, LayerData>& allLayers() const;

		// 修改图层专用接口
		bool setLayerData(const std::string& id, const LayerData& data);
		bool setLayerPosition(const std::string& id, Vec2f pos);
		bool setLayerAlpha(const std::string& id, float alpha);
		bool setLayerVisible(const std::string& id, bool visible);

		void setSpeaker(const std::string& name);
		void setContent(const std::string& text, bool append);
		std::string getSpeaker() const;
		std::string getContent() const;

	private:
		std::unordered_map<std::string, LayerData> m_layers;		// layerId -> LayerData
		std::string m_speaker;
		std::string m_content;

		// 内部通用修改器
		template <typename Func>
		bool modifyLayer(const std::string& id, Func&& updater) {
			auto it = m_layers.find(id);
			if (it == m_layers.end()) return false;
			bool changed = updater(it->second);
			if (changed) it->second.dirty = true;
			return true;
		}
	};

} // namespace endeus