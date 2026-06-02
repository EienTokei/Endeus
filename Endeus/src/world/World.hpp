#pragma once

#include <string>
#include <unordered_map>

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

		friend bool operator==(const LayerData& a, const LayerData& b) {
			return a.textureKey == b.textureKey
				&& a.order == b.order
				&& a.position == b.position
				&& a.alpha == b.alpha
				&& a.visible == b.visible
				&& a.texRect == b.texRect;
		}

		friend bool operator!=(const LayerData& a, const LayerData& b) {
			return !(a == b);
		}
	};

	struct World {
		std::unordered_map<std::string, LayerData> layers;		// layerId -> LayerData
		std::string speaker;
		std::string content;

		friend bool operator==(const World& a, const World& b) {
			return a.layers == b.layers
				&& a.speaker == b.speaker
				&& a.content == b.content;
		}

		friend bool operator!=(const World& a, const World& b) {
			return !(a == b);
		}
	};

} // namespace endeus