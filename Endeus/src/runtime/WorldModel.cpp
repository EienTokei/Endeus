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
		return modifyLayer(id, [&](LayerData& m_data) { m_data = data; });
	}
	bool WorldModel::setLayerPosition(const std::string& id, Vec2f pos) {
		return modifyLayer(id, [&](LayerData& data) { data.position = pos; });
	}
	bool WorldModel::setLayerAlpha(const std::string& id, float alpha) {
		return modifyLayer(id, [&](LayerData& data) { data.alpha = alpha; });
	}
	bool WorldModel::setLayerVisible(const std::string& id, bool visible) {
		return modifyLayer(id, [&](LayerData& data) { data.visible = visible; });
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

	void WorldModel::setChoice(const std::vector<ChoiceOption>& options) {
		m_choiceOptions = options;
	}

	void WorldModel::clearChoice() {
		m_choiceOptions.clear();
	}

	bool WorldModel::hasChoice() const {
		return !m_choiceOptions.empty();
	}

	const std::vector<ChoiceOption>& WorldModel::getChoiceOptions() const {
		return m_choiceOptions;
	}

} // namespace endeus
