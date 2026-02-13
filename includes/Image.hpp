#pragma once
#include <iostream>
#include <unordered_map>
#include <json.hpp>

struct Image {
	std::string uri;
	std::string mimeType;
	int bufferView = -1;
	std::string name;
	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;
};