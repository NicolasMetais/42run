#pragma once
#include <iostream>
#include <fstream>

#include <unordered_map>
#include <Font/AtlasFont.hpp>
#include <TextureManager.hpp>
#include <json.hpp>

class FontManager {
    private:
        std::unordered_map<std::string, AtlasFont> Fonts;
    public:
        void load(TextureManager& textureManager, std::string path, std::string name);
        const AtlasFont& getFont(const std::string& name) const;
};

