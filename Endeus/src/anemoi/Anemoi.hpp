#pragma once

#include <unordered_map>

#include "Anemos.hpp"

namespace endeus {

	/**
	 * @brief 让画面随风而动的动画系统
	 * 
	 * Anemoi（风神），致敬同名视觉小说。负责管理所有图层的动画
	 * 
	 * 每一帧更新动画状态，并提供覆盖值供渲染器使用。
	 */
	class Anemoi {
	public:
		/**
		 * @brief 添加一个动画到系统中。
		 *
		 * @param anemos 要添加的动画对象（AnemosVariant 类型）。
		 */
		void add(AnemosVariant anemos);

		/**
		 * @brief 更新所有正在播放的动画。
		 *
		 * @param dt 距离上一帧的时间差（秒）。
		 * @return true 如果仍有动画正在播放；false 如果所有动画均已结束。
		 */
		bool update(float dt);

		/**
		 * @brief 跳过所有正在播放的动画，直接到达它们的终点状态。
		 *
		 * 会立即完成每个动画的最终效果。
		 */
		void skipAll();

		/**
		 * @brief 重置所有动画到初始状态（不播放，仅重置内部状态）。
		 *
		 * 通常用于场景重置或重新开始动画序列。
		 */
		void resetAll();

		/**
		 * @brief 检查是否有动画正在播放。
		 *
		 * @return true 如果至少有一个动画未完成；false 否则。
		 */
		bool isPlaying() const;

		/**
		 * @brief 清除所有动画（停止并移除）。
		 *
		 * 清空内部动画容器，不会调用 skip/重置。
		 */
		void clear();

		/**
		 * @brief 获取当前所有动画的覆盖值（用于渲染时的临时覆盖）。
		 *
		 * 渲染器应读取这些覆盖值来改变图层位置、透明度等属性，
		 * 而不会污染 World 中的原始数据。
		 *
		 * @return 键（图层ID+属性）到覆盖值的映射。
		 */
		[[nodiscard]] std::unordered_map<AnemosKey, AnemosOverrides> getOverrides() const;
	private:
		std::unordered_map<AnemosKey, AnemosVariant> m_anemosMap;
	};

} // namespace endeus
