#pragma once

#include <functional>
#include <array>
#include <vector>

#include "Event.hpp"

namespace endeus {
	/**
	 * @brief 地脉总线 - 传递事件的网络
	 * 
	 * 灵感取自「时钟机关的Ley-Line」
	 */
	class Leyline {
	public:
		using Callback = std::function<void(const Event&)>;
		
		template<typename T>
		void subscribe(Callback cb) {
			constexpr size_t idx = Event::typeIndex<T>();
			m_callbacks[idx].emplace_back(std::move(cb));
		}

		template<typename T>
		void publish(const T& event) {
			Event e(event);
			size_t idx = e.index();
			for (auto& cb : m_callbacks[idx]) {
				cb(e);
			}
		}

		void clear() {
			for (auto& vec : m_callbacks) {
				vec.clear();
			}
		}

	private:
		static constexpr size_t N = Event::typeCount();
		std::array<std::vector<Callback>, N> m_callbacks;
	};
}

