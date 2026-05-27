#include "WorldManager.hpp"

namespace endeus {
	bool WorldManager::addLayer(const std::string& id, const LayerData& data) {
		auto it = m_world.layers.find(id);
		if (it != m_world.layers.end()) {
			return false;
		}
		m_world.layers[id] = data;
		m_world.layers[id].dirty = true;
		return true;
	}

	bool WorldManager::setLayerData(const std::string& id, const LayerData& data) {
		return modifyLayer(id, [&](LayerData& m_data) -> bool {
			if (m_data == data) return false;	// false 表示 unchanged
			m_data = data;
			return true;
		});
	}
	bool WorldManager::setLayerPosition(const std::string& id, Vec2f pos) {
		return modifyLayer(id, [&](LayerData& data) {
			if (data.position == pos) return false;
			data.position = pos;
			return true;
		});
	}
	bool WorldManager::setLayerAlpha(const std::string& id, float alpha) {
		return modifyLayer(id, [&](LayerData& data) {
			if (data.alpha == alpha) return false;
			data.alpha = alpha;
			return true;
		});
	}
	bool WorldManager::setLayerVisible(const std::string& id, bool visible) {
		return modifyLayer(id, [&](LayerData& data) {
			if (data.visible == visible) return false;
			data.visible = visible;
			return true;
		});
	}

	void WorldManager::removeLayer(const std::string& id) {
		m_world.layers.erase(id);
	}

	const LayerData* WorldManager::getLayer(const std::string& id) const {
		auto it = m_world.layers.find(id);
		if (it != m_world.layers.end()) {
			return &it->second;
		}
		return nullptr;
	}

	const std::unordered_map<std::string, LayerData>& WorldManager::allLayers() const {
		return m_world.layers;
	}

	void WorldManager::setSpeaker(const std::string& name) {
		m_world.speaker = name;
	}

	void WorldManager::setContent(const std::string& text, bool append) {
		if (append) {
			m_world.content += text;
		}
		else {
			m_world.content = text;
		}
	}

	std::string WorldManager::getSpeaker() const {
		return m_world.speaker;
	}

	std::string WorldManager::getContent() const {
		return m_world.content;
	}

	void WorldManager::setWorld(World&& world) {
		m_world = std::move(world);
	}

	const World& WorldManager::getWorld() const {
		return m_world;
	}

	//World WorldManager::mirror() const {
	//	World another;
	//	another = m_world;
	//	return another;
	//}

} // namespace endeus

