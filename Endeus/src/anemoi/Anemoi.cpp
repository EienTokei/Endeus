#include "Anemoi.hpp"
#include "../utils/Logger.hpp"

namespace endeus {
	void Anemoi::add(AnemosVariant anemos) {
		std::string layerId = std::visit([](auto& t) -> std::string {
			return t.layerId;
		}, anemos);
		auto index = anemos.index();		// 运行时索引
		AnemosKey key{ std::move(layerId), index };
		SPDLOG_TRACE("Anemoi add: layer='{}', property={}", key.layerId, key.propertyIndex);
		m_anemosMap[key] = std::move(anemos);
	}

	bool Anemoi::update(float dt) {
		size_t erased = std::erase_if(m_anemosMap, [](auto& pair) -> bool {
			return std::visit([](auto& anemos) {
				return anemos.isFinished();
			}, pair.second);
		});
		if (erased > 0) {
			SPDLOG_DEBUG("Anemoi update: {} animation(s) completed", erased);
		}
		for (auto& [_, variant] : m_anemosMap) {
			std::visit([dt](auto& anemos) {
				anemos.update(dt);
			}, variant);
		}
		// 不删除已完成动画, 避免渲染器未能绘制最终状态导致残留[#2]
		return !m_anemosMap.empty();
	}

	void Anemoi::skipAll() {
		SPDLOG_DEBUG("Anemoi skipAll: clearing {} animations", m_anemosMap.size());
		for (auto& [_, variant] : m_anemosMap) {
			std::visit([](auto& anemos) {
				anemos.skip();
			}, variant);
		}
		clear();
	}

	void Anemoi::resetAll() {
		SPDLOG_DEBUG("Anemoi resetAll: resetting {} animations", m_anemosMap.size());
		for (auto& [_, variant] : m_anemosMap) {
			std::visit([](auto& anemos) {
				anemos.reset();
			}, variant);
		}
		clear();
	}

	bool Anemoi::isPlaying() const {
		return !m_anemosMap.empty();
	}

	void Anemoi::clear() {
		SPDLOG_TRACE("Anemoi clear: clearing all animations");
		m_anemosMap.clear();
	}

	std::unordered_map<AnemosKey, AnemosOverrides> Anemoi::getOverrides() const {
		std::unordered_map<AnemosKey, AnemosOverrides> result;
		for (auto& [key, variant] : m_anemosMap) {
			result[key] = std::visit([](auto& anemos) {
				return anemos.getOverrides();
			}, variant);
		}
		return result;
	}

} // namespace endeus