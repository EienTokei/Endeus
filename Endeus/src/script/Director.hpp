#pragma once

#include "Instruction.hpp"
#include "../runtime/IRuntime.hpp"
#include "../event/EventBus.hpp"

#include <unordered_map>

namespace endeus {

	class Director {
	public:
		Director(IRuntime& runtime, EventBus& eventBus);
		void init(const std::vector<Instruction>& instructions);
		void update(float dt);
		bool isFinished() const;

	private:
		// 执行当前 m_pc 指向的指令
		void execute();
		void toNext();
		void toLabel(const std::string& label);
		void onEvent(const Event& e);

		IRuntime& m_runtime;
		EventBus& m_eventBus;
		std::vector<Instruction> m_instructions;
		std::unordered_map<std::string, size_t> m_labelMap;
		size_t m_pc = 0;
		bool m_finished = false;
		bool m_waiting = false;
	};

} // namespace endeus
