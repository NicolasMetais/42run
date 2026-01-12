#pragma once
#include <iostream>

struct Text {
	int sampler = -1;
	int source = -1;
	std::string name;
	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;

};