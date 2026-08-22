#pragma once
#include <iostream>

/** @brief OpenGL texture sampler parameters as defined in GLTF (filter modes and wrap modes). */
struct TextureSampler {
	int magFilter = 9729; // GL_LINEAR magnification filter by default.
	int minFilter = 9729; // GL_LINEAR minification filter by default.
	int wrapS = 10497;    // GL_REPEAT wrap mode on S axis by default.
	int wrapT = 10497;    // GL_REPEAT wrap mode on T axis by default.
	std::string name;
	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;
};
