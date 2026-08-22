#pragma once
#include <vector>
#include <unordered_map>
#include <cstdint>

/** @brief A GLTF mesh: a collection of primitives each with their own accessor references. */
struct Mesh {
	/** @brief A single renderable primitive (draw call unit) within a mesh. */
	struct Primitive {
		int positionAccessor = -1; // Accessor index for vertex positions.
		int normalAccessor = -1;   // Accessor index for vertex normals.
		int tangentAccessor = -1;  // Accessor index for vertex tangents.

		std::unordered_map<int, int> texcoords; // UV set index → accessor index.
		std::unordered_map<int, int> colors;    // Color set index → accessor index.
		std::unordered_map<int, int> joints;    // Joint set index → accessor index.
		std::unordered_map<int, int> weights;   // Weight set index → accessor index.

		int indexAccessor = -1;  // Accessor index for indices (-1 = non-indexed).
		int materialIndex = -1;  // Index into the model's material list.
	};
	std::vector<Primitive> primitives;
};
