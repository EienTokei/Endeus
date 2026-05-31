#include "WhiteAlbum.hpp"

namespace endeus {

	WhiteAlbum::WhiteAlbum(Director& director, WorldManager& worldManager) 
		: m_director(director), m_worldManager(worldManager){}

	void WhiteAlbum::memorize(const std::string& label) {
		m_leaves[label] = pickLeaf();
	}

	void WhiteAlbum::recall(const std::string& label) {
		turnToLeaf(m_leaves[label]);
	}

	bool WhiteAlbum::hasLeaf(const std::string& label) const {
		return m_leaves.find(label) != m_leaves.end();
	}

	Leaf WhiteAlbum::pickLeaf() const {
		return Leaf(m_director.takeMemento(), m_worldManager.takeMemento());
	}

	void WhiteAlbum::turnToLeaf(const Leaf& leaf) {
		m_director.recallMemento(leaf.director);
		m_worldManager.recallMemento(leaf.world);
	}


} // namespace endeus
