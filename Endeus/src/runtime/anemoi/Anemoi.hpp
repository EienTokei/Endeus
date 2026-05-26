#pragma once

#include <unordered_map>
#include <memory>

#include "IAnemos.hpp"

namespace endeus {

	/**
	 * @brief 让画面随风而动的动画系统
	 * 
	 * Anemoi: 风神, 致敬同名视觉小说
	 */
	class Anemoi {
	public:
		void add(std::unique_ptr<IAnemos> anemos);

		// true -> 还有动画播放
		bool update(float dt, WorldModel& world);

		void skipAll(WorldModel& world);

		void resetAll(WorldModel& world);

		bool isPlaying() const;

		void clear();

	private:
		std::unordered_map<AnemosKey, std::unique_ptr<IAnemos>> m_anemosMap;
	};

} // namespace endeus
