#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

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
		void handleInput();

		/**
		 * @brief 清空窗口并绘制所有可见元素
		 * @details 绘制背景、角色精灵、文本框、名字、对话文本、选项按钮
		 */
		void render();

		/**
		 * @brief 从 assets 目录加载所有纹理和字体
		 * @throws std::runtime_error 任一资源加载失败时抛出
		 */
		void loadAssets();

		/**
		 * @brief 初始化硬编码的剧本数据（首轮迭代）
		 * @details 设置初始背景、角色位置、对话内容，重置状态变量
		 */
		void runScripts();

		sf::RenderWindow window;
		sf::Font font;

		std::unordered_map<std::string, sf::Texture> textures;
		std::unique_ptr<sf::Sprite> background;
		std::unique_ptr<sf::Sprite> character;

		std::unique_ptr<sf::Text> nameText;
		std::unique_ptr<sf::Text> dialogueText;
		std::vector<sf::Text> choiceTexts;

		int currentStep = 0;
		bool waitingForChoice = false;
	};

	inline sf::String toSF(const char8_t* u8str) {
		auto len = std::char_traits<char8_t>::length(u8str);
		// 这里直接把内存中的二进制数据当作另一种类型来解释
		return sf::String::fromUtf8(reinterpret_cast<const char*>(u8str),
									reinterpret_cast<const char*>(u8str) + len);
	}
} // namespace endeus