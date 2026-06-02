#pragma once

#include <unordered_map>

#include "Anemos.hpp"

namespace endeus {

	/**
	 * @brief 让画面随风而动的动画系统
	 * 
	 * Anemoi: 风神, 致敬同名视觉小说
	 */
	class Anemoi {
	public:
		void add(AnemosVariant anemos);

		// true -> 还有动画播放
		bool update(float dt);

		void skipAll();

		void resetAll();

		bool isPlaying() const;

		void clear();

		[[nodiscard]] std::unordered_map<AnemosKey, AnemosOverrides> getOverrides() const;
	private:
		std::unordered_map<AnemosKey, AnemosVariant> m_anemosMap;
	};

} // namespace endeus
