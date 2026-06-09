#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "../script/Instruction.hpp"
#include "../director/Director.hpp"
#include "../executor/Executor.hpp"
#include "../anemoi/Anemoi.hpp"
#include "../world/WorldManager.hpp"
#include "../renderer/Renderer.hpp"
#include "../album/WhiteAlbum.hpp"


namespace endeus {

	/**
	 * @brief Endeus 引擎的核心类, 管理窗口/资源/脚本执行和主循环
	 */
	class Engine {
	public:
		/**
		 * @brief 构造函数，创建窗口、加载资源并初始化剧本
		 * @throws std::runtime_error 如果字体文件或必要图片缺失
		 */
		Engine();

		/**
		 * @brief 启动引擎主循环
		 * @details 进入事件循环，持续处理输入、更新状态、渲染，直到窗口关闭
		 */
		void run();
	private:
		/**
		 * @brief 处理所有待处理的 SFML 事件
		 * @details 包括窗口关闭、鼠标点击推进剧情或处理选项
		 */
		void processEvents();

		/**
		 * @brief 挂载硬编码的资源别名映射.
		 */
		void mountResources();

		/**
		 * @brief 从 assets 目录加载字体
		 * @throws std::runtime_error 任一资源加载失败时抛出
		 */
		void loadAssets();

		/**
		 * @brief 初始化硬编码的剧本数据
		 * @details 设置初始背景、角色位置、对话内容，重置状态变量
		 */
		void buildScripts();

		/**
		 * @brief 清理场景以进行跳转
		 * @details 停止所有音频、停止所有动画、清理渲染器中的临时状态
		 */
		void clearScene();

		/**
		 * @brief.处理 Director 返回的结果
		 * @param result Director update() 返回的动作
		 */
		void handleDirectorResult(const DirectorResult& result);

		WorldManager m_worldManager;
		sf::RenderWindow m_window;
		Leyline m_leyline;
		Executor m_executor;
		Renderer m_renderer;
		Director m_director;
		Anemoi m_anemoi;
		WhiteAlbum m_album;
		std::vector<Instruction> m_script;
	};

} // namespace endeus