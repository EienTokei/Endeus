#pragma once

#include "../script/Instruction.hpp"
#include "../executor/IExecutor.hpp"
#include "../event/Leyline.hpp"

#include <unordered_map>

namespace endeus {

	class Director {
	public:
		Director(IExecutor& executor, Leyline& leyline);
		void init(const std::vector<Instruction>& instructions);
		void update(float dt);
		bool isFinished() const;

	private:
		// 执行当前 m_pc 指向的指令
		void execute();
		void toNext();
		void toLabel(const std::string& label);
		void onEvent(const Event& e);

		IExecutor& m_executor;
		Leyline& m_leyline;
		std::vector<Instruction> m_instructions;
		std::unordered_map<std::string, size_t> m_labelMap;
		size_t m_pc = 0;
		bool m_finished = false;
		bool m_waiting = false;
	};

} // namespace endeus
