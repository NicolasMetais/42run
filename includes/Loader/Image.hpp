#pragma once
#include <iostream>
#include <unordered_map>
#include <json.hpp>

/** @brief Image source referenced by a GLTF texture (URI or embedded buffer view). */
struct Image {
	std::string uri;         ///< File path or data URI of the image.
	std::string mimeType;    ///< MIME type (e.g. "image/png") when embedded.
	int bufferView = -1;     ///< Buffer view index for embedded images (-1 if URI-based).
	std::string name;
	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;
};
