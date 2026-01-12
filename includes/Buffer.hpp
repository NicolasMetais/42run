#pragma once
#include <iostream>

struct Buffer {
	std::string uri;
	uint64_t byteLength;
	std::string name;

	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;
};