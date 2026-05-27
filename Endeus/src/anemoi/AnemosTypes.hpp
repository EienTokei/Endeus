#pragma once

#include <string>

namespace endeus {
	struct AnemosKey {
		std::string layerId;
		enum class Property { Position, Alpha } property;
	};

	inline bool operator==(const AnemosKey& a, const AnemosKey& b) {
		return a.layerId == b.layerId && a.property == b.property;
	}
}

// 为 std::hash 类模板增加一个 AnemosKey 的特化, 让 AnemosKey 可以直接作为 unordered_map 的键
namespace std {
	template <>		// 全特化, 为特定类型写的专用版本, 不是新模板
	struct hash<endeus::AnemosKey> {		// // 写明要特化的具体类型
		size_t operator()(const endeus::AnemosKey& key) const {
			return hash<string>()(key.layerId) ^ (static_cast<size_t>(key.property) << 1);
		}
	};
}
