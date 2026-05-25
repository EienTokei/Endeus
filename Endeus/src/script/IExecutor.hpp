#pragma once

#include "Instruction.hpp"

namespace endeus {

	class IExecutor {
	public:
		virtual ~IExecutor() = default;

		// 执行一条指令
		virtual bool execute(const Instruction& instr) = 0;

		// 每帧更新内部异步状态
		virtual void update(float dt) = 0;

		// 清空异步活动
		//virtual void resetAsync() = 0;
	};

} // namespace endeus