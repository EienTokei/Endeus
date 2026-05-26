#include "Director.hpp"

namespace endeus {
	Director::Director(IExecutor& executor, EventBus& eventBus)
		: m_executor(executor), m_eventBus(eventBus) {
		// 订阅事件
		m_eventBus.subscribe<Event::ActionCompleted>([this](const Event& e) { onEvent(e); });
		m_eventBus.subscribe<Event::ChoiceSelected>([this](const Event& e) { onEvent(e); });
	}

	void Director::init(const std::vector<Instruction>& instructions) {
		m_instructions = instructions;
		m_labelMap.clear();
		for (size_t i = 0; i < m_instructions.size(); ++i) {
			if (const auto* label = m_instructions[i].getIf<Instruction::Label>()) {
				m_labelMap[label->name] = i;
			}
		}
		m_pc = 0;
		m_finished = false;
		m_waiting = false;
	}

	void Director::update(float dt) {
		if (m_finished) {
			return;
		}
		m_executor.update(dt);
		if (m_waiting) {
			return;
		}
		execute();
	}

	bool Director::isFinished() const {
		return m_finished;
	}

	void Director::execute() {
		while (!m_finished && m_pc < m_instructions.size()) {
			const Instruction& instr = m_instructions[m_pc];
			if (instr.is<Instruction::Label>()) {
				toNext();
				continue;
			}
			if (const auto* jump = instr.getIf<Instruction::Jump>()) {
				toLabel(jump->targetLabel);
				continue;
			}
			if (instr.is<Instruction::End>()) {
				m_finished = true;
				break;
			}
			bool completed = m_executor.execute(instr);
			if(completed) {
				toNext();	// 同步 -> 继续循环, 执行下一条
			}
			else {
				m_waiting = true;
				break;		// 异步 -> 停止循环, 进入等待
			}
		}
	}

	void Director::toNext() {
		m_pc++;
		if (m_pc >= m_instructions.size()) {
			m_finished = true;
		}
	}

	void Director::toLabel(const std::string& label) {
		auto it = m_labelMap.find(label);
		if (it != m_labelMap.end()) {
			if (m_pc > it->second) {		// 这里简单处理: 当向前回滚时清空旧异步活动, 避免旧动画残留
				m_executor.resetAsync();
			}
			m_pc = it->second;
		}
		else {
			// 标签不存在，停止
			m_finished = true;
		}
	}

	void Director::onEvent(const Event& e) {
		if (auto* choice = e.getIf<Event::ChoiceSelected>()) {
			m_waiting = false;
			toLabel(choice->targetLabel);
			execute();
		}
		else if (e.is<Event::ActionCompleted>()) {
			m_waiting = false;
			toNext();
			execute();
		}
	}


}
