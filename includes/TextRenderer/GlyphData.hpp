#pragma once

struct GlyphData {
    int unicode; //character
    float advance; //horizontal size to advance after this char
    float planeLeft = 0; //planebound = positions in world
    float planeRight = 0;
    float planeBottom = 0;
    float planeTop = 0;
    float atlasLeft = 0; //atlasBound = positions in the png
    float atlasRight = 0;
    float atlasBottom = 0;
    float atlasTop = 0;
};