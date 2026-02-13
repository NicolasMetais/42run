#pragma once
#include <iostream>
#include <unordered_map>
#include <json.hpp>

struct Text {
	int sampler = -1;
	int source = -1;
	std::string name;
	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;

};