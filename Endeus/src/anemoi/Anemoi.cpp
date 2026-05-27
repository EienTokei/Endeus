#include "Anemoi.hpp"

namespace endeus {
	void Anemoi::add(std::unique_ptr<IAnemos> anemos) {
		auto key = anemos->getKey();
		m_anemosMap[key] = std::move(anemos);
	}

	bool Anemoi::update(float dt) {
		size_t erased = std::erase_if(m_anemosMap, [&](auto& pair) -> bool {
			return pair.second->update(dt);
		});
		if (erased == m_anemosMap.size()) {
			return false;
		}
		return true;
	}

	void Anemoi::skipAll() {
		for (auto& [_, anemos] : m_anemosMap) {
			anemos->skip();
		}
		clear();
	}

	void Anemoi::resetAll() {
		for (auto& [_, anemos] : m_anemosMap) {
			anemos->reset();
		}
		clear();
	}

	bool Anemoi::isPlaying() const {
		return !m_anemosMap.empty();
	}

	void Anemoi::clear() {
		m_anemosMap.clear();
	}

	const std::unordered_map<AnemosKey, std::unique_ptr<IAnemos>>& Anemoi::getAnemosMap() const {
		return m_anemosMap;
	}

} // namespace endeus