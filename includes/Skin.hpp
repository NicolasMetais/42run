#pragma once
#include <iostream>
#include <vector>

struct Skin {
    std::string name;

    std::vector<int> joints;
    int inverseBindMatrixAccessor = -1;
    int skeletonRoot = -1;
};