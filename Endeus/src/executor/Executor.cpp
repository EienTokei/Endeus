#include "Executor.hpp"
#include "../anemoi/Anemos.hpp"
#include "../utils/Logger.hpp"

namespace endeus {
	Executor::Executor(WorldManager& worldManager, Anemoi& anemoi) : m_worldManager(worldManager), m_anemoi(anemoi){}

	bool Executor::execute(const Instruction& instr) {
		if (auto* show = instr.getIf<Instruction::ShowLayer>()) {
			return handleShowLayer(*show);
		}
		if (auto* hide = instr.getIf<Instruction::HideLayer>()) {
			return handleHideLayer(*hide);
		}
		if (auto* move = instr.getIf<Instruction::MoveLayer>()) {
			return handleMoveLayer(*move);	// 不阻塞 Director 调度, 继续推进剧情
		}
		if (auto* fade = instr.getIf<Instruction::FadeLayer>()) {
			return handleFadeLayer(*fade);	// 不阻塞 Director 调度, 继续推进剧情
		}
		if (auto* speaker = instr.getIf<Instruction::SetSpeaker>()) {
			return handleSetSpeaker(*speaker);
		}
		if (auto* content = instr.getIf<Instruction::SetContent>()) {
			return handleSetContent(*content);
		}
		if (auto* choice = instr.getIf<Instruction::Choice>()) {
			return handleChoice(*choice);
		}
		SPDLOG_WARN("Executor: unknown instruction type");
		return true;
	}

	void Executor::resetAsync() {
		SPDLOG_DEBUG("Executor resetAsync: clearing all animations");
		m_anemoi.resetAll();
	}

	bool Executor::handleShowLayer(const Instruction::ShowLayer& instr) {
		SPDLOG_DEBUG("ShowLayer: id='{}', texture='{}', pos=({},{}), alpha={}, order={}",
					 instr.layerId, instr.textureKey,
					 instr.position.x, instr.position.y, instr.alpha, instr.order);
		LayerData data{ instr.textureKey, instr.order, true, instr.position, instr.alpha , instr.texRect };

		if (!m_worldManager.addLayer(instr.layerId, data)) {
			SPDLOG_DEBUG("ShowLayer: layer '{}' already exists, updating data", instr.layerId);
			m_worldManager.setLayerData(instr.layerId, data);
		}
		return true;
	}

	bool Executor::handleHideLayer(const Instruction::HideLayer& instr) {
		SPDLOG_DEBUG("HideLayer: id='{}'", instr.layerId);
		m_worldManager.setLayerVisible(instr.layerId, false);
		return true;
	}

	bool Executor::handleMoveLayer(const Instruction::MoveLayer& instr) {
		std::string id = instr.layerId;
		const LayerData* layer = m_worldManager.getLayer(id);
		if (!layer) {
			SPDLOG_WARN("MoveLayer: layer '{}' does not exist, skipping", id);
			return true;
		}

		SPDLOG_DEBUG("MoveLayer: id='{}', from ({}, {}) to ({}, {}), duration={}s",
					 id, layer->position.x, layer->position.y,
					 instr.toPosition.x, instr.toPosition.y, instr.durationSeconds);
		MoveAnemos anemos{
			id,
			layer->position,		// from
			instr.toPosition,		// to
			layer->position,		// current
			instr.durationSeconds,
			0.0f
		};
		m_anemoi.add(std::move(anemos));

		m_worldManager.setLayerPosition(id, instr.toPosition);		// 直接记录最终效果, 插值由动画系统负责
		return true;
	}

	bool Executor::handleFadeLayer(const Instruction::FadeLayer& instr) {
		std::string id = instr.layerId;
		const LayerData* layer = m_worldManager.getLayer(id);
		if (!layer) {
			SPDLOG_WARN("FadeLayer: layer '{}' does not exist, skipping", id);
			return true;
		}

		SPDLOG_DEBUG("FadeLayer: id='{}', from alpha {} to {}, duration={}s",
					 id, layer->alpha, instr.toAlpha, instr.durationSeconds);
		FadeAnemos anemos{
			id,
			layer->alpha,		// from
			instr.toAlpha,		// to
			layer->alpha,		// current
			instr.durationSeconds, 
			0.0f
		};
		m_anemoi.add(std::move(anemos));

		m_worldManager.setLayerAlpha(id, instr.toAlpha);		// 直接记录最终效果, 插值由动画系统负责
		return true;
	}

	bool Executor::handleSetSpeaker(const Instruction::SetSpeaker& instr) {
		SPDLOG_TRACE("SetSpeaker: '{}'", instr.name);
		m_worldManager.setSpeaker(instr.name);
		return true;
	}

	bool Executor::handleSetContent(const Instruction::SetContent& instr) {
		// 日志预览内容
		std::string preview = instr.content;
		if (preview.size() > 15) {
			preview = preview.substr(0, 12) + "...";
		}
		SPDLOG_DEBUG("SetContent: append={}, text='{}'", instr.append, preview);
		m_worldManager.setContent(instr.content, instr.append);
		return true;
	}

	bool Executor::handleChoice(const Instruction::Choice& instr) {
		// 日志预览选项
		std::string optionsStr;
		for (size_t i = 0; i < instr.options.size(); ++i) {
			if (i > 0) optionsStr += ", ";
			optionsStr += instr.options[i].text;
		}
		SPDLOG_INFO("Choice: {} options -> [{}]", instr.options.size(), optionsStr);
		m_worldManager.setOptions(instr.options);
		return false;
	}

} // namespace endeus