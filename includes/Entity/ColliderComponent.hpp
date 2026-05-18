#pragma once
#include <vector>
#include <Matrix/Matrix.hpp>

struct ColliderComponent {
    struct Box {
        Vector<float> min = {0, 0, 0};
        Vector<float> max = {0, 0, 0};
    };

    std::vector<Box> boxes;
    bool isTrigger = false;
};
