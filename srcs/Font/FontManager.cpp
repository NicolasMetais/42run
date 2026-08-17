#include <Font/FontManager.hpp>

namespace { //namespace here has the same effect as static
    template<typename T>
    T getField(const nlohmann::json& json , const std::string& key) {
        if(!json.contains(key))
            throw std::runtime_error("Missing field" + key + "in police json");
        return json[key].get<T>();
    }
}

void FontManager::load(TextureManager& textureManager, std::string path, std::string name) {
    textureManager.getOrLoad(name + ".png");
    nlohmann::json police;
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open json's police");
    file >> police;

    AtlasFont newFont;

    newFont.width = getField<int>(police["atlas"], "width");
    newFont.height = getField<int>(police["atlas"], "height");
    newFont.size = getField<int>(police["atlas"], "size");
    newFont.distanceRange = getField<float>(police["atlas"], "distanceRange");
    newFont.lineHeight = getField<float>(police["metrics"], "lineHeight");
    newFont.ascender = getField<float>(police["metrics"], "ascender");
    newFont.descender = getField<float>(police["metrics"], "descender");

    for (auto& glyph : police["glyphs"]) { //loop in every police char
        GlyphData newGlyph;

        newGlyph.unicode = getField<int>(glyph, "unicode");
        newGlyph.advance = getField<float>(glyph, "advance");
        if (glyph.contains("planeBounds")) {
            newGlyph.planeLeft = getField<float>(glyph["planeBounds"], "left");
            newGlyph.planeRight = getField<float>(glyph["planeBounds"], "right");
            newGlyph.planeBottom = getField<float>(glyph["planeBounds"], "bottom");
            newGlyph.planeTop = getField<float>(glyph["planeBounds"], "top");
            newGlyph.atlasLeft = getField<float>(glyph["atlasBounds"], "left");
            newGlyph.atlasRight = getField<float>(glyph["atlasBounds"], "right");
            newGlyph.atlasBottom = getField<float>(glyph["atlasBounds"], "bottom");
            newGlyph.atlasTop = getField<float>(glyph["atlasBounds"], "top");
        }
        newFont.glyphs[newGlyph.unicode] = newGlyph;
    }
    Fonts[name] = newFont;
};

const AtlasFont& FontManager::getFont(const std::string& name) const {
    return Fonts.at(name);
};

