#include "AssetManager.hpp"

namespace endeus {
	AssetManager& AssetManager::getInstance() {
		static AssetManager instance;
		return instance;
	}

	void AssetManager::mount(const std::string& alias, const std::string& path) {
		m_aliasToPath[alias] = path;
		SPDLOG_TRACE("AssetManager: mounted '{}' -> '{}'", alias, path);
	}

	void AssetManager::unmount(const std::string& alias) {
		m_aliasToPath.erase(alias);
		SPDLOG_TRACE("AssetManager: unmounted '{}'", alias);
	}

	std::string AssetManager::resolveAlias(const std::string& alias) const {
		auto it = m_aliasToPath.find(alias);
		if (it == m_aliasToPath.end()) {
			SPDLOG_TRACE("resolveAlias: no alias for '{}', using as path", alias);
			return alias;
		}
		SPDLOG_TRACE("resolveAlias: resolved alias '{}' -> '{}'", alias, it->second);
		return it->second;
	}

	ResourceHandle<sf::Texture> AssetManager::getTexture(const std::string& aliasOrPath) {
		return m_textureCache.load(resolveAlias(aliasOrPath));
	}

	ResourceHandle<sf::SoundBuffer> AssetManager::getSoundBuffer(const std::string& aliasOrPath) {
		return m_soundCache.load(resolveAlias(aliasOrPath));
	}

	void AssetManager::clearAll() {
		m_textureCache.clear();
		m_soundCache.clear();
		SPDLOG_DEBUG("clearAll: cleared");
	}


} // namespace endeus
