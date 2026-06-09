#pragma once

#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace endeus {

	/**
	 * @brief 音频管理器
	 * 
	 * 取自「想要传达给你的爱恋」中的同名乐队
	 * 
	 * 你听到了你想听的星之声吗
	 */
	class GloriousDays {
	public:
		/**
		 * .
		 */
		static GloriousDays& getInstance();		// 音频硬件全局唯一且任何模块都可能随时播放声音，单例提供最直接、无依赖的访问方式。

		void playBGM(const std::filesystem::path& filepath);

		void stopBGM();

		void playSE(const std::filesystem::path& filepath);

		void playVO(const std::filesystem::path& filepath);

		void stopVO();

		void setBGMVolume(float volume);
		void setSEVolume(float volume);
		void setVOVolume(float volume);

		void update();

		void clear();

	private:
		GloriousDays() = default;
		~GloriousDays() = default;

		sf::Music m_bgm;
		float m_bgmVolume = 100.f;

		std::vector<sf::Sound> m_activeSEs;
		float m_seVolume = 100.f;
		std::unordered_map<std::string, sf::SoundBuffer> m_seBuffers;

		std::optional<sf::Sound> m_vo;
		std::unordered_map<std::string, sf::SoundBuffer> m_voBuffers;
		float m_voVolume = 100.f;
	};
}
