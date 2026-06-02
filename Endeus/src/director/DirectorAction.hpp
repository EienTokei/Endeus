#pragma once

#include <string>
#include <optional>

namespace endeus {

	enum class DirectorAction {
		Terminated,		// 结束
		Waiting,		// 等待异步事件
		Memorize,		// 需要保存当前快照
		Recall		// 恢复到目标快照
	};

	struct DirectorResult {
		DirectorAction action;
		std::optional<std::string> targetLabel;
	};

} // namespace endeus
