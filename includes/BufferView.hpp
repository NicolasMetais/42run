#pragma once
#include <cstdint>
#include <json.hpp>

struct BufferView {
	uint32_t buffer = 0;
	uint32_t byteOffset = 0;
	uint32_t byteLength = 0;
	uint32_t byteStride = 0;
	uint32_t target = 0;
	
	std::string name = "";
	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;

	uint8_t* base = nullptr;
};