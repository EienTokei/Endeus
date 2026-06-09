#include "GloriousDays.hpp"
#include "../utils/Logger.hpp"

namespace endeus {
	GloriousDays& GloriousDays::getInstance() {
		static GloriousDays instance;
		return instance;
	}

	void GloriousDays::playBGM(const std::filesystem::path& filepath) {
		if (!m_bgm.openFromFile(filepath)) {
			SPDLOG_ERROR("Failed to load BGM: {}", filepath.string());
			return;
		}
		m_bgm.setLooping(true);
		m_bgm.setVolume(m_bgmVolume);
		m_bgm.play();
		SPDLOG_DEBUG("BGM started: {}", filepath.string());
	}

	void GloriousDays::stopBGM() {
		m_bgm.stop();
		SPDLOG_DEBUG("BGM stopped");
	}

	void GloriousDays::playSE(const std::filesystem::path& filepath) {
		auto it = m_seBuffers.find(filepath.string());
		if (it == m_seBuffers.end()) {
			sf::SoundBuffer buffer;
			if (!buffer.loadFromFile(filepath)) {
				SPDLOG_ERROR("Failed to load SE: {}", filepath.string());
				return;
			}
			it = m_seBuffers.emplace(filepath.string(), std::move(buffer)).first;
		}

		// 直接在容器中构造，SFML 的 sf::Sound 移动构造函数没有转移播放状态
		auto& sound = m_activeSEs.emplace_back(it->second);
		sound.setVolume(m_seVolume);
		sound.play();

		SPDLOG_DEBUG("SE played: {}", filepath.string());
	}

	void GloriousDays::playVO(const std::filesystem::path& filepath) {
		auto it = m_voBuffers.find(filepath.string());
		if (it == m_voBuffers.end()) {
			sf::SoundBuffer buffer;
			if (!buffer.loadFromFile(filepath)) {
				SPDLOG_ERROR("Failed to load VO: {}", filepath.string());
				return;
			}
			it = m_voBuffers.emplace(filepath.string(), std::move(buffer)).first;
		}

		if (!m_vo.has_value()) {
			m_vo.emplace(it->second);
		}
		else {
			m_vo->setBuffer(it->second);
		}
		m_vo->setVolume(m_voVolume);
		m_vo->play();

		SPDLOG_DEBUG("VO played: {}", filepath.string());
	}

	void GloriousDays::stopVO() {
		m_vo->stop();
		SPDLOG_DEBUG("VO stopped");
	}

	void GloriousDays::setBGMVolume(float volume) {
		m_bgmVolume = std::clamp(volume, 0.f, 100.f);
		m_bgm.setVolume(m_bgmVolume);
	}

	void GloriousDays::setSEVolume(float volume) {
		m_seVolume = std::clamp(volume, 0.f, 100.f);
		for (auto& se : m_activeSEs) {
			se.setVolume(m_seVolume);
		}
	}

	void GloriousDays::setVOVolume(float volume) {
		m_voVolume = std::clamp(volume, 0.f, 100.f);
		if (m_vo.has_value()) {
			m_vo->setVolume(m_voVolume);
		}
	}

	void GloriousDays::update() {
		size_t erased = std::erase_if(m_activeSEs, [](const auto& se) {
			return se.getStatus() == sf::Sound::Status::Stopped;
		});
		if (erased > 0) {
			SPDLOG_DEBUG("GloriousDays update: {} SE(s) completed", erased);
		}
	}

	void GloriousDays::clear() {
		m_activeSEs.clear();
		m_seBuffers.clear();
		m_bgm.stop();
		SPDLOG_DEBUG("Cleared all sound caches and stopped all playing sounds");
	}

} // namespace endeus
