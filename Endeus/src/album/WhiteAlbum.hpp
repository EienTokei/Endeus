#pragma once

#include "../director/Director.hpp"
#include "../world/WorldManager.hpp"

#include <string>
#include <unordered_map>

namespace endeus {

	// 全局快照(相簿的一页) - 包含各个模块的 Memento
	struct Leaf {
		Director::Memento director;
		WorldManager::Memento world;
	};

	/**
	 * @brief 白色相簿 - 历史状态的管理者（Caretaker）
	 * 
	 * 相簿里的每一页都承载着过去的回忆, 令人怀念, 致敬同名作品「白色相簿」
	 * 不断收集场景，填满这本相册吧
	 */
	class WhiteAlbum {
	public:
		WhiteAlbum(Director& director, WorldManager& worldManager);

		// 保存当前状态到相册，并用 label 标记这一页（覆盖旧页）
		void memorize(const std::string& label);

		// 回忆 label 标记的那一页（恢复到该状态）
		void recall(const std::string& label);

		bool hasLeaf(const std::string& label) const;

	private:
		WorldManager& m_worldManager;
		Director& m_director;
		std::unordered_map<std::string, Leaf> m_leaves;		// label -> Leaf

		// 取下一片叶
		Leaf pickLeaf() const;

		// 翻到指定页
		void turnToLeaf(const Leaf& leaf);
	};

} // namespace endeus
