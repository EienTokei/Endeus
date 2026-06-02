#pragma once

#include <variant>
#include "AnemosTypes.hpp"
#include "../utils/Interpolate.hpp"

namespace endeus {

	/// 风, 代指动画
	template<typename T>
	struct Anemos {
		std::string layerId;
		T from;
		T to;
		T current;
		float duration = 0;
		float elapsed = 0;

		void update(float dt) {
			elapsed += dt;
			float t = interp::normalize(elapsed, duration);
			current = interp::lerp(from, to, t);
		}
		void skip() {
			current = to;
			elapsed = duration;
		}
		void reset() {
			current = from;
			elapsed = 0;
		}

		bool isFinished() {
			return elapsed >= duration;
		}
	};

	struct MoveAnemos : Anemos<Vec2f> {
		[[nodiscard]] AnemosOverrides getOverrides() const {
			return { .position = current };
		}
	};

	struct FadeAnemos : Anemos<float> {
		[[nodiscard]] AnemosOverrides getOverrides() const {
			return { .alpha = current };
		}
	};

	using AnemosVariant = std::variant<
		MoveAnemos,
		FadeAnemos
	>;
}
