#pragma once

#include "DirectorAction.hpp"
#include "../script/Instruction.hpp"
#include "../executor/IExecutor.hpp"
#include "../event/Leyline.hpp"

#include <unordered_map>

namespace endeus {

	class Director {
	public:
		Director(IExecutor& executor, Leyline& leyline);
		void init(const std::vector<Instruction>& instructions);
		[[nodiscard]] DirectorResult update(float dt);
		bool isFinished() const;

		struct Memento {
			size_t pc = 0;
		};
		[[nodiscard]] Memento takeMemento() const;
		void recallMemento(const Memento& memento);


	private:
		// 从当前 m_pc 指向的指令开始推进
		[[nodiscard]] DirectorResult advance();
		void toNext();
		bool toFuture(const std::string& label);	// 迈向未来
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
