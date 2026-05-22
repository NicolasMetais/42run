#pragma once
#include <unordered_map>
#include <TextRenderer/GlyphData.hpp>

struct AtlasFont {
    int width;
    int height;
    int size;
    float lineHeight;
    float ascender;
    float descender;
    std::unordered_map<int, GlyphData> glyphs;
};