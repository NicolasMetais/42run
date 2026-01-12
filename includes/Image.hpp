#pragma once
#include <iostream>

struct Image {
	std::string uri;
	std::string mimeType;
	int bufferView = -1;
	std::string name;
	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;
};