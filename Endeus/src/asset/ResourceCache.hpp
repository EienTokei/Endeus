#pragma once

#include "ResourceHandle.hpp"
#include "../utils/Logger.hpp"

#include <unordered_map>
#include <string>
#include <concepts>

namespace endeus {

	// sf::Font/Music 的接口是 openFromFile()，不过这些通常也不需要缓存，可能是 SFML 的设计意图吧
	template<typename T>
	concept LoadableFromFile = requires(T obj, const std::string& path) {
		T{};
		{ obj.loadFromFile(path) } -> std::convertible_to<bool>;
	};

	template<LoadableFromFile T>
	class ResourceCache {
		std::unordered_map<std::string, std::shared_ptr<T>> m_cache;	// path -> resource

	public:
		ResourceHandle<T> load(const std::string& path) {
			auto itRes = m_cache.find(path);
			if (itRes != m_cache.end()) {
				SPDLOG_TRACE("loadResource: path={}, cached=true", path);
				return ResourceHandle<T>(itRes->second);
			}
			SPDLOG_DEBUG("loadResource: path={}, cached=false", path);
			auto resource = std::make_shared<T>();
			if (!resource->loadFromFile(path)) {
				SPDLOG_ERROR("loadResource: path={}, error=loadFromFile failed", path);
				return {};
			}

			m_cache[path] = resource;
			SPDLOG_DEBUG("loadResource: path={}, success=true", path);
			return ResourceHandle<T>(resource);
		}

		void clear() {
			m_cache.clear();
			SPDLOG_TRACE("clear: cache cleared");
		}
	};

}// namespace endeus
