#include "WorldModel.hpp"

namespace endeus {
	void WorldModel::setLayer(const std::string& id, const LayerData& data) {
		auto it = m_layers.find(id);
		if (it != m_layers.end() && it->second == data) {
			// 数据相同，无需标记脏
			return;
		}
		m_layers[id] = data;
		m_layers[id].dirty = true;
	}

	void WorldModel::hideLayer(const std::string& id) {
		auto it = m_layers.find(id);
		if (it != m_layers.end()) {
			it->second.visible = false;
			it->second.dirty = true;
		}
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
