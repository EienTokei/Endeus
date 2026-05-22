#pragma once

#include "../script/Instruction.hpp"

namespace endeus {

	class IRuntime {
	public:
		virtual ~IRuntime() = default;

		// 执行一条指令
		virtual bool execute(const Instruction& instr) = 0;

		// 每帧更新内部异步状态
		virtual void update(float dt) = 0;
	};

} // namespace endeus