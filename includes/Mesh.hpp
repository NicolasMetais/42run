#pragma once
#include <vector>
#include <unordered_map>
#include <cstdint>

struct Mesh {
	struct Primitive {
		int positionAccessor = -1;
		int normalAccessor = -1;
		int tangentAccessor = -1;

		std::unordered_map<int, int> texcoords;
		std::unordered_map<int, int> colors;
		std::unordered_map<int, int> joints;
		std::unordered_map<int, int> weights;

		int indexAccessor = -1;
		int materialIndex = -1;
	};
	std::vector<Primitive> primitives;
};