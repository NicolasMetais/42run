#pragma once
#include <iostream>

struct TextureSampler {
	int magFilter = 9729;
	int minFilter = 9729;
	int wrapS = 10497;
	int wrapT = 10497;
	std::string name;
	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;

};