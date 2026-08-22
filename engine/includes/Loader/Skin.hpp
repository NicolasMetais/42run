#pragma once
#include <iostream>
#include <vector>

/** @brief A GLTF skin: a set of joints with inverse bind matrices for skeletal animation. */
struct Skin {
    std::string name;
    std::vector<int> joints;            // Node indices that act as joints.
    int inverseBindMatrixAccessor = -1; // Accessor index for the inverse bind matrices.
    int skeletonRoot = -1;              // Node index of the skeleton root (-1 = inferred).
};
