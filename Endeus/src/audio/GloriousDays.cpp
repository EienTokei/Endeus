#include "GloriousDays.hpp"
#include "../utils/Logger.hpp"
#include "../asset/AssetManager.hpp"

namespace endeus {
	GloriousDays& GloriousDays::getInstance() {
		static GloriousDays instance;
		return instance;
	}

	void GloriousDays::playBGM(const std::string& alias) {
		auto& assets = AssetManager::getInstance();
		std::string filepath = assets.resolveAlias(alias);
		if (!m_bgm.openFromFile(filepath)) {
			SPDLOG_ERROR("Failed to load BGM: {}", filepath);
			return;
		}
		m_bgm.setLooping(true);
		m_bgm.setVolume(m_bgmVolume);
		m_bgm.play();
		SPDLOG_DEBUG("BGM started: {}", alias);
	}

	void GloriousDays::stopBGM() {
		m_bgm.stop();
		SPDLOG_DEBUG("BGM stopped");
	}

	void GloriousDays::playSE(const std::string& alias) {
		auto& assets = AssetManager::getInstance();
		auto bufferHandle = assets.getSoundBuffer(alias);
		if (!bufferHandle) {
			SPDLOG_ERROR("Failed to get sound buffer for SE: {}", alias);
			return;
		}

		// 直接在容器中构造，SFML 的 sf::Sound 移动构造函数没有转移播放状态
		auto& sound = m_activeSEs.emplace_back(*bufferHandle.get());
		sound.setVolume(m_seVolume);
		sound.play();

		SPDLOG_DEBUG("SE played: {}", alias);
	}

	void GloriousDays::playVO(const std::string& alias) {
		auto& assets = AssetManager::getInstance();
		auto bufferHandle = assets.getSoundBuffer(alias);
		if (!bufferHandle) {
			SPDLOG_ERROR("Failed to get sound buffer for VO: {}", alias);
			return;
		}

		if (!m_vo.has_value()) {
			m_vo.emplace(*bufferHandle.get());
		}
		else {
			m_vo->setBuffer(*bufferHandle.get());
		}
		m_vo->setVolume(m_voVolume);
		m_vo->play();

		SPDLOG_DEBUG("VO played: {}", alias);
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
		m_bgm.stop();
		SPDLOG_DEBUG("Stopped all playing sounds");
	}

} // namespace endeus
