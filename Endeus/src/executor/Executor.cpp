#include "Executor.hpp"
#include "../anemoi/MoveLayerAnemos.hpp"
#include "../anemoi/FadeLayerAnemos.hpp"

#include <iostream>

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
		// 暂不实现
		return true;
	}

	void Executor::resetAsync() {
		m_anemoi.resetAll();
	}

	bool Executor::handleShowLayer(const Instruction::ShowLayer& instr) {
		LayerData data{ instr.textureKey, instr.order, true, instr.position, instr.alpha , instr.texRect };

		if (!m_worldManager.addLayer(instr.layerId, data)) {
			m_worldManager.setLayerData(instr.layerId, data);
		}
		return true;
	}

	bool Executor::handleHideLayer(const Instruction::HideLayer& instr) {
		m_worldManager.setLayerVisible(instr.layerId, false);
		return true;
	}

	bool Executor::handleMoveLayer(const Instruction::MoveLayer& instr) {
		std::string id = instr.layerId;
		const LayerData* layer = m_worldManager.getLayer(id);
		if (!layer) {
			std::cerr << "move: no layer - " << id << std::endl;
			return true;
		}

		auto anemos = std::make_unique<MoveLayerAnemos>(
			id,
			m_worldManager.getLayer(id)->position,
			instr.toPosition,
			instr.durationSeconds
		);
		m_anemoi.add(std::move(anemos));

		m_worldManager.setLayerPosition(id, instr.toPosition);		// 直接记录最终效果, 插值由动画系统负责
		return true;
	}

	bool Executor::handleFadeLayer(const Instruction::FadeLayer& instr) {
		std::string id = instr.layerId;
		const LayerData* layer = m_worldManager.getLayer(id);
		if (!layer) {
			std::cerr << "move: no layer - " << id << std::endl;
			return true;
		}

		auto anemos = std::make_unique<FadeLayerAnemos>(
			id,
			m_worldManager.getLayer(id)->alpha,
			instr.toAlpha,
			instr.durationSeconds
		);
		m_anemoi.add(std::move(anemos));

		m_worldManager.setLayerAlpha(id, instr.toAlpha);		// 直接记录最终效果, 插值由动画系统负责
		return true;
	}

	bool Executor::handleSetSpeaker(const Instruction::SetSpeaker& instr) {
		m_worldManager.setSpeaker(instr.name);
		return true;
	}

	bool Executor::handleSetContent(const Instruction::SetContent& instr) {
		m_worldManager.setContent(instr.content, instr.append);
		return true;
	}

	bool Executor::handleChoice(const Instruction::Choice& instr) {
		m_worldManager.setOptions(instr.options);
		return false;
	}

} // namespace endeus