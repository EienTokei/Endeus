#pragma once

#include "../anemoi/Anemoi.hpp"
#include "../world/WorldManager.hpp"
#include "IExecutor.hpp"

namespace endeus {

	class Executor : public IExecutor {
	public:
		Executor(WorldManager& worldManager, Anemoi& anemoi);

		[[nodiscard]] bool execute(const Instruction& instr) override;

		void resetAsync() override;

	private:
		WorldManager& m_worldManager;
		Anemoi& m_anemoi;

		// true - 同步完成; false - 需要等待
		bool handleShowLayer(const Instruction::ShowLayer& instr);
		bool handleHideLayer(const Instruction::HideLayer& instr);
		bool handleMoveLayer(const Instruction::MoveLayer& instr);
		bool handleFadeLayer(const Instruction::FadeLayer& instr);
		bool handlePlayBGM(const Instruction::PlayBGM& instr);
		bool handlePlaySE(const Instruction::PlaySE& instr);
		bool handlePlayVO(const Instruction::PlayVO& instr);
		bool handleSetSpeaker(const Instruction::SetSpeaker& instr);
		bool handleSetContent(const Instruction::SetContent& instr);
		bool handleChoice(const Instruction::Choice& instr);
	};

} // namespace endeus