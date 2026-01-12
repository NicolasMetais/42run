#pragma once
#include <iostream>

struct Node {
	std::string name;
	int mesh = -1;
	int camera = -1;
	std::vector<int> children;

	std::vector<float> translation = {0.0f, 0.0f, 0.0f};
	std::vector<float> rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	std::vector<float> scale = {1.0f, 1.0f, 1.0f};
	std::vector<float> matrix;

	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;
};