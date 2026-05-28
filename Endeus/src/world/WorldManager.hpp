#pragma once

#include <string>
#include <unordered_map>

#include "World.hpp"
#include "../utils/Types.hpp"
#include "../script/Instruction.hpp"

namespace endeus {

	class WorldManager {
	public:
		bool addLayer(const std::string& id, const LayerData& data);
		void removeLayer(const std::string& id);
		const LayerData* getLayer(const std::string& id) const;
		const std::unordered_map<std::string, LayerData>& allLayers() const;

		// 修改图层专用接口
		bool setLayerData(const std::string& id, const LayerData& data);
		bool setLayerPosition(const std::string& id, Vec2f pos);
		bool setLayerAlpha(const std::string& id, float alpha);
		bool setLayerVisible(const std::string& id, bool visible);

		void setSpeaker(const std::string& name);
		void setContent(const std::string& text, bool append);
		std::string getSpeaker() const;
		std::string getContent() const;

		void setOptions(const std::vector<Instruction::Choice::Option>& opts) { m_options = opts; }
		void clearOptions() { m_options.clear(); }
		const auto& getOptions() const { return m_options; }
		std::vector<Instruction::Choice::Option> takeOptions() {
			return std::move(m_options);
		}

		void setWorld(World&& world);
		const World& getWorld() const;
		//World mirror() const;			// 另一个世界

	private:
		World m_world;
		std::vector<Instruction::Choice::Option> m_options;

		// 内部通用修改器
		template <typename Func>
		bool modifyLayer(const std::string& id, Func&& updater) {
			auto it = m_world.layers.find(id);
			if (it == m_world.layers.end()) return false;
			bool changed = updater(it->second);
			if (changed) it->second.dirty = true;
			return true;
		}
	};

} // namespace endeus