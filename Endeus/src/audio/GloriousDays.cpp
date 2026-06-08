#include "GloriousDays.hpp"
#include "../utils/Logger.hpp"

namespace endeus {
	GloriousDays& GloriousDays::getInstance() {
		static GloriousDays instance;
		return instance;
	}

	void GloriousDays::playBGM(const std::string& filepath) {
		if (!m_bgm.openFromFile(filepath)) {
			SPDLOG_ERROR("Failed to load BGM: {}", filepath);
			return;
		}
		m_bgm.setLooping(true);
		m_bgm.setVolume(m_bgmVolume);
		m_bgm.play();
		SPDLOG_DEBUG("BGM started: {}", filepath);
	}

	void GloriousDays::stopBGM() {
		m_bgm.stop();
		SPDLOG_DEBUG("BGM stopped");
	}

	void GloriousDays::playSE(const std::string& filepath) {
		auto it = m_soundBuffers.find(filepath);
		if (it == m_soundBuffers.end()) {
			sf::SoundBuffer buffer;
			if (!buffer.loadFromFile(filepath)) {
				SPDLOG_ERROR("Failed to load SE: {}", filepath);
				return;
			}
			it = m_soundBuffers.emplace(filepath, std::move(buffer)).first;
		}

		sf::Sound sound(it->second);
		sound.setVolume(m_seVolume);
		sound.play();

		m_activeSEs.emplace_back(std::move(sound));
		SPDLOG_DEBUG("SE played: {}", filepath);
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

	void GloriousDays::update() {
		std::erase_if(m_activeSEs, [](const auto& se) {
			return se.getStatus() == sf::Sound::Status::Stopped;
		});
	}

	void GloriousDays::clear() {
		m_activeSEs.clear();
		m_soundBuffers.clear();
		m_bgm.stop();
		SPDLOG_DEBUG("Cleared all sound caches and stopped all playing sounds");
	}

} // namespace endeus
