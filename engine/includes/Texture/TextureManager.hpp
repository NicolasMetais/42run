#pragma once
#include <iostream>
#include <unordered_map>
#include <Texture/Texture.hpp>

/** @brief Cache of loaded textures keyed by URI, avoiding redundant GPU uploads. */
class TextureManager {
	private:
		std::unordered_map<std::string, Texture> cache; ///< O(1) lookup by file path or URI.
	public:
		/** @brief Returns the cached texture for @p uri, loading and caching it on first access. */
		Texture& getOrLoad(const std::string& uri);
		/** @brief Destroys all cached textures and clears the cache. */
		void clear();
};
