#include "Anemoi.hpp"
#include "../utils/Logger.hpp"

namespace endeus {
	void Anemoi::add(std::unique_ptr<IAnemos> anemos) {
		auto key = anemos->getKey();
		SPDLOG_TRACE("Anemoi add: layer='{}', property={}",
					 key.layerId, static_cast<int>(key.property));
		m_anemosMap[key] = std::move(anemos);
	}

	bool Anemoi::update(float dt) {
		size_t erased = std::erase_if(m_anemosMap, [&](auto& pair) -> bool {
			return pair.second->update(dt);
		});
		if (erased > 0) {
			SPDLOG_TRACE("Anemoi update: {} animation(s) completed", erased);
		}
		return !m_anemosMap.empty();
	}

	void Anemoi::skipAll() {
		SPDLOG_DEBUG("Anemoi skipAll: clearing {} animations", m_anemosMap.size());
		for (auto& [_, anemos] : m_anemosMap) {
			anemos->skip();
		}
		clear();
	}

	void Anemoi::resetAll() {
		SPDLOG_DEBUG("Anemoi resetAll: resetting {} animations", m_anemosMap.size());
		for (auto& [_, anemos] : m_anemosMap) {
			anemos->reset();
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

	const std::unordered_map<AnemosKey, std::unique_ptr<IAnemos>>& Anemoi::getAnemosMap() const {
		return m_anemosMap;
	}

} // namespace endeus