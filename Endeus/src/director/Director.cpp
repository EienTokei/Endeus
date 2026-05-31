#include "Director.hpp"

#include <iostream>
namespace endeus {
	Director::Director(IExecutor& executor, Leyline& leyline)
		: m_executor(executor), m_leyline(leyline) {
		// 订阅事件
		m_leyline.subscribe<Event::ActionCompleted>([this](const Event& e) { onEvent(e); });
		m_leyline.subscribe<Event::ChoiceSelected>([this](const Event& e) { onEvent(e); });
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

	DirectorResult Director::update(float dt) {
		if (m_finished) {
			return DirectorResult(DirectorAction::Terminated);
		}
		if (m_waiting) {
			return DirectorResult(DirectorAction::Waiting);
		}
		return advance();
	}

	bool Director::isFinished() const {
		return m_finished;
	}

	Director::Memento Director::takeMemento() const {
		return Memento(m_pc);
	}

	void Director::recallMemento(const Memento& memento) {
		m_pc = memento.pc;
		m_waiting = false;		// 恢复后不等待, 否则会跳过 Label 后的第一个指令
	}

	DirectorResult Director::advance() {
		while (!m_finished && m_pc < m_instructions.size()) {
			const Instruction& instr = m_instructions[m_pc];
			if (const auto* label = instr.getIf<Instruction::Label>()) {
				std::cout << "Director: reached label " << label->name << std::endl;
				toNext();
				return DirectorResult(DirectorAction::Memorize, label->name);
			}
			if (const auto* jump = instr.getIf<Instruction::Jump>()) {
				if (toFuture(jump->targetLabel)) {
					continue;
				}
				else {
					return DirectorResult(DirectorAction::Recall, jump->targetLabel);
				}
			}
			if (instr.is<Instruction::End>()) {
				m_finished = true;
				return DirectorResult(DirectorAction::Terminated);
			}
			if (instr.is<Instruction::Wait>()) {
				m_waiting = true;
				return DirectorResult(DirectorAction::Waiting);
			}
			bool completed = m_executor.execute(instr);
			if(completed) {
				toNext();	// 同步 -> 继续循环, 执行下一条
			}
			else {
				m_waiting = true;
				//break;		// 异步 -> 停止循环, 进入等待
				return DirectorResult(DirectorAction::Waiting);
			}
		}
		return DirectorResult(DirectorAction::Terminated);		// 只有可能 m_finished 为 true 或 m_pc 执行完会跳出循环执行到这里
	}

	void Director::toNext() {
		m_pc++;
		if (m_pc >= m_instructions.size()) {
			m_finished = true;
		}
	}

	bool endeus::Director::toFuture(const std::string& label) {
		auto it = m_labelMap.find(label);
		if (it != m_labelMap.end()) {
			if (m_pc >= it->second) {
				return false;		// 在过去
			}
			m_pc = it->second;
		}
		else {
			// 标签不存在，停止
			m_finished = true;
		}
		return true;
	}

	void Director::onEvent(const Event& e) {
		if (auto* choice = e.getIf<Event::ChoiceSelected>()) {
			m_waiting = false;
			toFuture(choice->targetLabel);
		}
		else if (e.is<Event::ActionCompleted>()) {
			m_waiting = false;
			toNext();
		}
	}


}
