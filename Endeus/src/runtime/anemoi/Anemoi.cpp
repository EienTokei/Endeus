#include "Anemoi.hpp"

namespace endeus {
	void Anemoi::add(std::unique_ptr<IAnemos> anemos) {
		auto key = anemos->getKey();
		m_anemosMap[key] = std::move(anemos);
	}

	bool Anemoi::update(float dt, WorldModel& world) {
		size_t erased = std::erase_if(m_anemosMap, [&](auto& pair) -> bool {
			return pair.second->update(dt, world);
		});
		if (erased == m_anemosMap.size()) {
			return false;
		}
		return true;
	}

	void Anemoi::skipAll(WorldModel& world) {
		for (auto& [_, anemos] : m_anemosMap) {
			anemos->skip(world);
		}
		clear();
	}

	bool Anemoi::isPlaying() const {
		return !m_anemosMap.empty();
	}

	void Anemoi::clear() {
		m_anemosMap.clear();
	}
} // namespace endeus