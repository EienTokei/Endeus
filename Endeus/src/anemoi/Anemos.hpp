#pragma once

#include <variant>
#include "AnemosTypes.hpp"
#include "../utils/Interpolate.hpp"

namespace endeus {

	/**
	 * @brief 风 —— 代表一个泛型动画。
	 *
	 * 模板类，封装了从起始值到目标值的动画，支持线性插值。
	 *
	 * @tparam T 动画属性的类型。
	 */
	template<typename T>
	struct Anemos {
		std::string layerId;	///< 所作用的图层标识符
		T from;					///< 起始值
		T to;					///< 目标值
		T current;				///< 当前插值结果
		float duration = 0;		///< 动画总时长（秒）
		float elapsed = 0;		///< 已经过的时间（秒）

		/**
		 * @brief 根据时间差更新动画状态。
		 *
		 * 增加已过时间，重新计算当前值（线性插值）。
		 *
		 * @param dt 距离上一帧的时间差（秒）。
		 */
		void update(float dt) {
			elapsed += dt;
			float t = interp::normalize(elapsed, duration);
			current = interp::lerp(from, to, t);
		}

		/**
		 * @brief 跳过动画，直接到达目标值。
		 *
		 * 将当前值设为目标值，已过时间设为时长，标记动画完成。
		 */
		void skip() {
			current = to;
			elapsed = duration;
		}

		/**
		 * @brief 重置动画到起始状态。
		 *
		 * 当前值恢复为起始值，已过时间归零。
		 */
		void reset() {
			current = from;
			elapsed = 0;
		}

		/**
		 * @brief 检查动画是否已完成。
		 *
		 * @return true 如果已过时间达到或超过时长；false 否则。
		 */
		bool isFinished() {
			return elapsed >= duration;
		}
	};

	/**
	 * @brief 移动动画（图层位置变化）。
	 *
	 * 继承自 Anemos<Vec2f>，提供位置覆盖值。
	 */
	struct MoveAnemos : Anemos<Vec2f> {
		/**
		 * @brief 获取当前动画覆盖值。
		 *
		 * @return AnemosOverrides 包含当前位置的覆盖结构。
		 */
		[[nodiscard]] AnemosOverrides getOverrides() const {
			return { .position = current };
		}
	};

	/**
	 * @brief 淡变动画（图层透明度变化）。
	 *
	 * 继承自 Anemos<float>，提供透明度覆盖值。
	 */
	struct FadeAnemos : Anemos<float> {
		/**
		 * @brief 获取当前动画覆盖值。
		 *
		 * @return AnemosOverrides 包含当前透明度的覆盖结构。
		 */
		[[nodiscard]] AnemosOverrides getOverrides() const {
			return { .alpha = current };
		}
	};

	/**
	 * @brief 支持的动画类型变体。
	 *
	 * 使用 std::variant 实现类型安全的动画存储与多态。
	 */
	using AnemosVariant = std::variant<
		MoveAnemos,
		FadeAnemos
	>;
}
