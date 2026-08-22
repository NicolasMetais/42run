#pragma once
#include <iostream>

/** @brief A node in the GLTF scene graph, holding a local transform and optional mesh/skin/camera references. */
struct Node {
	std::string name;
	int mesh = -1;    // Index of the attached mesh (-1 = none).
	int camera = -1;  // Index of the attached camera (-1 = none).
	int skin = -1;    // Index of the attached skin (-1 = none).
	std::vector<int> children; // Indices of child nodes.

	std::vector<float> translation = {0.0f, 0.0f, 0.0f};
	std::vector<float> rotation = {0.0f, 0.0f, 0.0f, 1.0f}; ///< Quaternion (x, y, z, w).
	std::vector<float> scale = {1.0f, 1.0f, 1.0f};
	std::vector<float> matrix; // Column-major 4x4 matrix override (used instead of TRS when present).

	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;
};
