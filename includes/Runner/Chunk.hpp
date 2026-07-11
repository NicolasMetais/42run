#pragma once
#include <iostream>
#include <vector>
#include <ECS.hpp>

// Le sol du chunk fait 42 unités de large (demi-largeur 3 du node Ground * scale 7),
// soit 6 lanes de 7. centerX(0) = -17.5 ... centerX(5) = +17.5, centré sur x = 0.
namespace Lane {
    constexpr int   COUNT = 6;
    constexpr float WIDTH = 7.0f;
    inline float centerX(int lane) { return (lane - (COUNT - 1) * 0.5f) * WIDTH; }
}

struct Chunk {
    std::vector<EntityId> ids;
    float zPos;
};