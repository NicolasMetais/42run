#pragma once
#include <unordered_map>
#include <Font/GlyphData.hpp>

struct AtlasFont {
    int width;
    int height;
    int size;
    float lineHeight;
    float ascender;
    float descender;
    float distanceRange; // Plage (en texels d'atlas) sur laquelle la distance MSDF est encodee.
    std::unordered_map<int, GlyphData> glyphs;
};