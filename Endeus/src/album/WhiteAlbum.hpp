#pragma once

#include "../director/Director.hpp"
#include "../world/WorldManager.hpp"

#include <string>
#include <unordered_map>

namespace endeus {

	/**
	 * @brief 全局快照(相簿的一页) - 包含各个模块的 Memento.
	 */
	struct Leaf {
		Director::Memento director;		///< 导演模块的 Memento
		WorldManager::Memento world;	///< 世界管理器的 Memento
	};

	/**
	 * @brief 白色相簿 - 历史状态的管理者（Caretaker）
	 * 
	 * 相簿里的每一页都承载着过去的回忆, 令人怀念, 致敬同名作品「白色相簿」
	 * 
	 * 不断收集场景，填满这本相册吧
	 */
	class WhiteAlbum {
	public:
		/**
		 * @brief 构造白色相簿对象。
		 *
		 * @param director     导演模块的引用，用于获取/恢复其备忘录。
		 * @param worldManager 世界管理器的引用，用于获取/恢复其备忘录。
		 */
		WhiteAlbum(Director& director, WorldManager& worldManager);

		/**
		 * @brief 保存当前状态到相册，并用指定标签标记这一页。
		 *
		 * 如果该标签已存在，则覆盖旧的快照。
		 *
		 * @param label 与该快照关联的标签（如场景名、章节点）。
		 */
		void memorize(const std::string& label);

		/**
		 * @brief 回忆（恢复）指定标签所标记的状态。
		 *
		 * 将导演模块和世界管理器恢复到该标签对应的历史快照。
		 *
		 * @param label 要恢复的状态标签。
		 * @pre hasLeaf(label) 为 true。
		 */
		void recall(const std::string& label);

		/**
		 * @brief 检查相册中是否已存在指定标签的快照。
		 *
		 * @param label 要检查的标签。
		 * @return true 如果存在；false 否则。
		 */
		bool hasLeaf(const std::string& label) const;

	private:
		Director& m_director;
		WorldManager& m_worldManager;
		std::unordered_map<std::string, Leaf> m_leaves;		// label -> Leaf

		// 取下一片叶
		Leaf pickLeaf() const;

		// 翻到指定页
		void turnToLeaf(const Leaf& leaf);
	};

} // namespace endeus
