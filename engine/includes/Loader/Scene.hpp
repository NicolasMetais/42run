#pragma once
#include <iostream>
#include <vector>

/** @brief A named GLTF scene: a list of root node indices forming a scene graph. */
struct Scene {
	std::string name;
	std::vector<int> rootNodes; // Indices of top-level nodes in this scene.
};
