#include "Anemoi.hpp"
#include "../utils/Logger.hpp"

namespace endeus {
	void Anemoi::add(AnemosVariant anemos) {
		// std::visit + 泛型 lambda 会在编译期为每个子类型实例化一个重载，运行时根据索引直接调用对应版本
		// auto& 会被推导为具体的子类型引用而不是整个 variant
		std::string layerId = std::visit([](auto& t) -> std::string {
			return t.layerId;
		}, anemos);
		auto index = anemos.index();		// 运行时索引
		AnemosKey key{ std::move(layerId), index };
		SPDLOG_TRACE("Anemoi add: layer='{}', property={}", key.layerId, key.propertyIndex);
		m_anemosMap[key] = std::move(anemos);
	}

	bool Anemoi::update(float dt) {
		// 移除上一帧完成的动画
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
		// 不删除本帧完成的动画, 避免渲染器未能绘制最终状态导致残留[#2]
		return !m_anemosMap.empty();
	}

	void Anemoi::skipAll() {
		SPDLOG_DEBUG("Anemoi skipAll: clearing {} animations", m_anemosMap.size());
		for (auto& [_, variant] : m_anemosMap) {
			std::visit([](auto& anemos) {
				anemos.skip();
			}, variant);
		}
	}

	void Anemoi::resetAll() {
		SPDLOG_DEBUG("Anemoi resetAll: resetting {} animations", m_anemosMap.size());
		for (auto& [_, variant] : m_anemosMap) {
			std::visit([](auto& anemos) {
				anemos.reset();
			}, variant);
		}
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