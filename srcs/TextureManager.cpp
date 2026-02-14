#include <TextureManager.hpp>

Texture& TextureManager::getOrLoad(const std::string& uri) {
	auto it = cache.find(uri);
	if (it != cache.end())
		return it->second;
	
	Texture tex;
	tex.loadTexture(uri);
	tex.openGl2DTextureGen();
	auto [itt, inserted] = cache.emplace(uri, tex);
	return itt->second;
};

void TextureManager::clear() {
	cache.clear();
};
