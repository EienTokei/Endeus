#pragma once

#include "../script/Instruction.hpp"

namespace endeus {

	class IExecutor {
	public:
		virtual ~IExecutor() = default;

		// 执行一条指令
		[[nodiscard]] virtual bool execute(const Instruction& instr) = 0;

		// 清空异步活动
		virtual void resetAsync() = 0;
	};

} // namespace endeus