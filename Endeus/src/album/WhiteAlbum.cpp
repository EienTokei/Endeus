#include "WhiteAlbum.hpp"
#include "../utils/Logger.hpp"

namespace endeus {

	WhiteAlbum::WhiteAlbum(Director& director, WorldManager& worldManager) 
		: m_director(director), m_worldManager(worldManager){
		SPDLOG_DEBUG("WhiteAlbum created");
	}

	void WhiteAlbum::memorize(const std::string& label) {
		bool existed = (m_leaves.find(label) != m_leaves.end());
		SPDLOG_DEBUG("Memorize: label='{}', existed={}", label, existed);
		m_leaves[label] = pickLeaf();
	}

	void WhiteAlbum::recall(const std::string& label) {
		auto it = m_leaves.find(label);
		if (it == m_leaves.end()) {
			SPDLOG_WARN("Recall: label='{}' not found in album", label);
			return;
		}
		SPDLOG_DEBUG("Recall: label='{}'", label);
		turnToLeaf(it->second);
	}

	bool WhiteAlbum::hasLeaf(const std::string& label) const {
		bool exists = (m_leaves.find(label) != m_leaves.end());
		SPDLOG_TRACE("hasLeaf: label='{}', exists={}", label, exists);
		return exists;
	}

	Leaf WhiteAlbum::pickLeaf() const {
		SPDLOG_TRACE("pickLeaf: taking director and world mementos");
		return Leaf(m_director.takeMemento(), m_worldManager.takeMemento());
	}

	void WhiteAlbum::turnToLeaf(const Leaf& leaf) {
		SPDLOG_TRACE("turnToLeaf: restoring director and world mementos");
		m_director.recallMemento(leaf.director);
		m_worldManager.recallMemento(leaf.world);
	}


} // namespace endeus
