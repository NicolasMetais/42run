#pragma once
#include <vector>
#include <string>
#include <Matrix/Matrix.hpp>

struct ColliderComponent {
    struct Box {
        Vector<float> min = {0, 0, 0};
        Vector<float> max = {0, 0, 0};
        std::string name; ///< nom du node gltf d'origine, pour router par prefixe (ex. OBSTACLE_*)
    };

    std::vector<Box> boxes;
    bool isTrigger = false;
};
