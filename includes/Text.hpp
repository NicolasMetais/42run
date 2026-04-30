#pragma once
#include <iostream>
#include <unordered_map>
#include <json.hpp>

/** @brief A GLTF texture descriptor: references a sampler and an image source by index. */
struct Text {
	int sampler = -1; ///< Index into the model's sampler list (-1 = default).
	int source = -1;  ///< Index into the model's image list.
	std::string name;
	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;
};
