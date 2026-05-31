#include "Director.hpp"
#include "../utils/Logger.hpp"

namespace endeus {
	Director::Director(IExecutor& executor, Leyline& leyline)
		: m_executor(executor), m_leyline(leyline) {
		// 订阅事件
		m_leyline.subscribe<Event::ActionCompleted>([this](const Event& e) { onEvent(e); });
		m_leyline.subscribe<Event::ChoiceSelected>([this](const Event& e) { onEvent(e); });
		SPDLOG_DEBUG("Director subscribed to ActionCompleted and ChoiceSelected events");
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
		SPDLOG_INFO("Director initialized: {} instructions, {} labels", m_instructions.size(), m_labelMap.size());
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
		SPDLOG_TRACE("takeMemento: pc={}", m_pc);
		return Memento(m_pc);
	}

	void Director::recallMemento(const Memento& memento) {
		SPDLOG_DEBUG("recallMemento: restoring pc from {} to {}", m_pc, memento.pc);
		m_pc = memento.pc;
		m_waiting = false;		// 恢复后不等待, 否则会跳过 Label 后的第一个指令
	}

	DirectorResult Director::advance() {
		while (!m_finished && m_pc < m_instructions.size()) {
			const Instruction& instr = m_instructions[m_pc];
			if (const auto* label = instr.getIf<Instruction::Label>()) {
				SPDLOG_DEBUG("Reached label '{}' at pc={}", label->name, m_pc);
				toNext();
				return DirectorResult(DirectorAction::Memorize, label->name);
			}
			if (const auto* jump = instr.getIf<Instruction::Jump>()) {
				SPDLOG_DEBUG("Jump instruction to label '{}' at pc={}", jump->targetLabel, m_pc);
				if (toFuture(jump->targetLabel)) {
					continue;
				}
				else {
					return DirectorResult(DirectorAction::Recall, jump->targetLabel);
				}
			}
			if (instr.is<Instruction::End>()) {
				SPDLOG_INFO("End instruction reached, finishing");
				m_finished = true;
				return DirectorResult(DirectorAction::Terminated);
			}
			if (instr.is<Instruction::Wait>()) {
				SPDLOG_TRACE("Wait instruction, entering waiting state");
				m_waiting = true;
				return DirectorResult(DirectorAction::Waiting);
			}
			SPDLOG_TRACE("Executing instruction type index {}", instr.index());
			bool completed = m_executor.execute(instr);
			if(completed) {
				toNext();	// 同步 -> 继续循环, 执行下一条
			}
			else {
				SPDLOG_TRACE("Instruction async, waiting");
				m_waiting = true;
				//break;		// 异步 -> 停止循环, 进入等待
				return DirectorResult(DirectorAction::Waiting);
			}
		}
		SPDLOG_DEBUG("advance finished: pc={}, finished={}", m_pc, m_finished);
		return DirectorResult(DirectorAction::Terminated);		// 只有可能 m_finished 为 true 或 m_pc 执行完会跳出循环执行到这里
	}

	void Director::toNext() {
		m_pc++;
		if (m_pc >= m_instructions.size()) {
			m_finished = true;
		}
		SPDLOG_TRACE("toNext: {} -> {}, finished={}", oldPc, m_pc, m_finished);
	}

	bool endeus::Director::toFuture(const std::string& label) {
		auto it = m_labelMap.find(label);
		if (it != m_labelMap.end()) {
			if (m_pc >= it->second) {
				SPDLOG_DEBUG("toFuture: label '{}' is in the past (pc={} >= target={})", label, m_pc, it->second);
				return false;		// 在过去
			}
			SPDLOG_DEBUG("toFuture: jumping to future label '{}' at pc={}", label, it->second);
			m_pc = it->second;
		}
		else {
			SPDLOG_ERROR("toFuture: label '{}' not found", label);
			// 标签不存在，停止
			m_finished = true;
		}
		return true;
	}

	void Director::onEvent(const Event& e) {
		if (auto* choice = e.getIf<Event::ChoiceSelected>()) {
			SPDLOG_DEBUG("onEvent: ChoiceSelected to label '{}'", choice->targetLabel);
			m_waiting = false;
			toFuture(choice->targetLabel);
		}
		else if (e.is<Event::ActionCompleted>()) {
			SPDLOG_TRACE("onEvent: ActionCompleted");
			m_waiting = false;
			toNext();
		}
	}


}
