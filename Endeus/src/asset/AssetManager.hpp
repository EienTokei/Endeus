#pragma once

#include "ResourceCache.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

namespace endeus {

	class AssetManager {
	public:
		static AssetManager& getInstance();

		void mount(const std::string& alias, const std::string& path);
		void unmount(const std::string& alias);
		std::string resolveAlias(const std::string& alias) const;

		ResourceHandle<sf::Texture> getTexture(const std::string& aliasOrPath);
		ResourceHandle<sf::SoundBuffer> getSoundBuffer(const std::string& aliasOrPath);

		void clearAll();

	private:
		std::unordered_map<std::string, std::string> m_aliasToPath;
		ResourceCache<sf::Texture> m_textureCache;
		ResourceCache<sf::SoundBuffer> m_soundCache;
	};

} // namespace endeus