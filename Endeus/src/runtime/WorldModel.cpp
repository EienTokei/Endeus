#include "WorldModel.hpp"

namespace endeus {
	bool WorldModel::addLayer(const std::string& id, const LayerData& data) {
		auto it = m_layers.find(id);
		if (it != m_layers.end()) {
			return false;
		}
		m_layers[id] = data;
		m_layers[id].dirty = true;
		return true;
	}

	bool WorldModel::setLayerData(const std::string& id, const LayerData& data) {
		return modifyLayer(id, [&](LayerData& m_data) -> bool { 
			if (m_data == data) return false;	// false 表示 unchanged
			m_data = data;
			return true;
		});
	}
	bool WorldModel::setLayerPosition(const std::string& id, Vec2f pos) {
		return modifyLayer(id, [&](LayerData& data) { 
			if (data.position == pos) return false;
			data.position = pos; 
			return true;
		});
	}
	bool WorldModel::setLayerAlpha(const std::string& id, float alpha) {
		return modifyLayer(id, [&](LayerData& data) {
			if (data.alpha == alpha) return false;
			data.alpha = alpha;
			return true;
		});
	}
	bool WorldModel::setLayerVisible(const std::string& id, bool visible) {
		return modifyLayer(id, [&](LayerData& data) {
			if (data.visible == visible) return false;
			data.visible = visible;
			return true;
		});
	}

	//void WorldModel::removeLayer(const std::string& id) {
	//	m_layers.erase(id);
	//}

	const LayerData* WorldModel::getLayer(const std::string& id) const {
		auto it = m_layers.find(id);
		if (it != m_layers.end()) {
			return &it->second;
		}
		return nullptr;
	}

	const std::unordered_map<std::string, LayerData>& WorldModel::allLayers() const {
		return m_layers;
	}

	void WorldModel::setSpeaker(const std::string& name) {
		m_speaker = name;
	}

	void WorldModel::setContent(const std::string& text, bool append) {
		if (append) {
			m_content += text;
		}
		else {
			m_content = text;
		}
	}

	std::string WorldModel::getSpeaker() const {
		return m_speaker;
	}

	std::string WorldModel::getContent() const {
		return m_content;
	}

	WorldModel WorldModel::mirror() const {
		WorldModel another;
		another.m_content = m_content;
		another.m_layers = m_layers;
		another.m_speaker = m_speaker;
		return another;
	}

} // namespace endeus
