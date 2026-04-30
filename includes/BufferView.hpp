#pragma once
#include <cstdint>
#include <json.hpp>

/** @brief A contiguous slice of a Buffer with optional interleaving stride. */
struct BufferView {
	uint32_t buffer = 0;      ///< Index of the parent buffer.
	uint32_t byteOffset = 0;  ///< Byte offset from the start of the buffer.
	uint32_t byteLength = 0;  ///< Length of the view in bytes.
	uint32_t byteStride = 0;  ///< Stride between elements (0 = tightly packed).
	uint32_t target = 0;      ///< OpenGL buffer target hint (e.g. GL_ARRAY_BUFFER).

	std::string name = "";
	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;

	uint8_t* base = nullptr;  ///< Resolved pointer into the loaded binary data.
};
