#pragma once
#include <iostream>

/** @brief Raw binary buffer as defined in the GLTF asset (URI or embedded). */
struct Buffer {
	std::string uri;           ///< Path or data URI of the buffer source.
	uint64_t byteLength;       ///< Total size of the buffer in bytes.
	std::string name;

	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;
};
