#pragma once
#include <iostream>

struct Sampler {
	int magFilter = 9729;
	int minFilter = 9729;
	int wrapS = 9729;
	int wrapT = 9729;
	std::string name;
	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;

};