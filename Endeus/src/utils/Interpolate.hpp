#pragma once

#include <algorithm>

namespace endeus::interp {

	// 线性插值
	template <typename T>
	inline T lerp(const T& a, const T& b, float t) {
		return a * (1.0f - t) + b * t;
	}

	inline float normalize(float elapsed, float duration) {
		if (duration <= 0.0f) return 1.0f;
		return std::clamp(elapsed / duration, 0.0f, 1.0f);		// [ 0.0f, 1.0f ]
	}
}
