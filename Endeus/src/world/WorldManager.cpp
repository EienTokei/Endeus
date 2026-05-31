#include "WorldManager.hpp"
#include "../utils/Logger.hpp"

namespace endeus {
	bool WorldManager::addLayer(const std::string& id, const LayerData& data) {
		auto it = m_world.layers.find(id);
		if (it != m_world.layers.end()) {
			SPDLOG_DEBUG("addLayer: layer '{}' already exists, not adding", id);
			return false;
		}
		m_world.layers[id] = data;
		m_world.layers[id].dirty = true;
		SPDLOG_DEBUG("addLayer: id='{}', texture='{}', order={}, pos=({},{}), alpha={}",
					 id, data.textureKey, data.order, data.position.x, data.position.y, data.alpha);
		return true;
	}

	bool WorldManager::setLayerData(const std::string& id, const LayerData& data) {
		return modifyLayer(id, [&](LayerData& m_data) -> bool {
			if (m_data == data) return false;	// false 表示 unchanged
			m_data = data;
			SPDLOG_DEBUG("setLayerData: id='{}', texture='{}', order={}, pos=({},{}), alpha={}",
						 id, data.textureKey, data.order, data.position.x, data.position.y, data.alpha);
			return true;
		});
	}
	bool WorldManager::setLayerPosition(const std::string& id, Vec2f pos) {
		return modifyLayer(id, [&](LayerData& data) {
			if (data.position == pos) return false;
			SPDLOG_TRACE("setLayerPosition: id='{}', pos=({},{}) -> ({},{})",
						 id, data.position.x, data.position.y, pos.x, pos.y);
			data.position = pos;
			return true;
		});
	}
	bool WorldManager::setLayerAlpha(const std::string& id, float alpha) {
		return modifyLayer(id, [&](LayerData& data) {
			if (data.alpha == alpha) return false;
			SPDLOG_TRACE("setLayerAlpha: id='{}', alpha={} -> {}", id, data.alpha, alpha);
			data.alpha = alpha;
			return true;
		});
	}
	bool WorldManager::setLayerVisible(const std::string& id, bool visible) {
		return modifyLayer(id, [&](LayerData& data) {
			if (data.visible == visible) return false;
			SPDLOG_TRACE("setLayerVisible: id='{}', visible={} -> {}", id, data.visible, visible);
			data.visible = visible;
			return true;
		});
	}

	void WorldManager::removeLayer(const std::string& id) {
		auto it = m_world.layers.find(id);
		if (it != m_world.layers.end()) {
			SPDLOG_DEBUG("removeLayer: id='{}'", id);
			m_world.layers.erase(it);
		}
		else {
			SPDLOG_WARN("removeLayer: layer '{}' not found", id);
		}
	}

	const LayerData* WorldManager::getLayer(const std::string& id) const {
		auto it = m_world.layers.find(id);
		if (it != m_world.layers.end()) {
			return &it->second;
		}
		SPDLOG_TRACE("getLayer: layer '{}' not found", id);
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
			SPDLOG_DEBUG("setContent: append mode");
			m_world.content += text;
		}
		else {
			SPDLOG_DEBUG("setContent: replace mode");
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
		SPDLOG_DEBUG("setWorld: replacing entire world (layers count={})", world.layers.size());
		m_world = std::move(world);
	}

	const World& WorldManager::getWorld() const {
		return m_world;
	}

	World WorldManager::mirror() const {
		SPDLOG_TRACE("mirror: creating a copy of current world");
		World another;
		another = m_world;
		return another;
	}

	WorldManager::Memento WorldManager::takeMemento() const {
		SPDLOG_DEBUG("takeMemento: saving current world state");
		return Memento(mirror());
	}

	void WorldManager::recallMemento(const Memento& memento) {
		SPDLOG_DEBUG("recallMemento: restoring world state (layers count={})", memento.world.layers.size());
		m_world = memento.world;
	}

} // namespace endeus

