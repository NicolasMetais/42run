#pragma once
#include <iostream>
#include <unordered_map>
#include <Texture.hpp>

class TextureManager {
	private:
		std::unordered_map<std::string, Texture> cache; //hashmap de texture O(1)
	public:
		Texture& getOrLoad(const std::string& uri);
		void clear();

};