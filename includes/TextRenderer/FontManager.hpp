#pragma once
#include <iostream>
#include <fstream>

#include <unordered_map>
#include <TextRenderer/AtlasFont.hpp>
#include <TextureManager.hpp>
#include <json.hpp>

class FontManager {
    private:
        std::unordered_map<std::string, AtlasFont> Fonts;
    public:
        void load(TextureManager& textureManager, std::string path, std::string name);
        AtlasFont getFont(const std::string& name);
};

