#pragma once
#include <iostream>
#include <Matrix/Matrix.hpp>

struct TextureInfo {
	int index = -1;
	int texCoord = 0;
	float scale = 1.0f;
	float strength = 1.0f;
};

struct pbrMetallicRoughness {
	std::vector<float> baseColorFactor = {1.0f, 1.0f, 1.0f, 1.0f};
	TextureInfo baseColorTexture;
	float metallicFactor = -1;
	float roughnessFactor = -1;
	TextureInfo metallicRoughnessTexture;
};

struct Mat {
	std::string name;
	pbrMetallicRoughness pbrMetallicRoughness;
	TextureInfo normalTexture;
	float normalTextureScale = 1.0f;
	TextureInfo occlusionTexture;
	float occlusionStrength = 1.0f;
	TextureInfo emissiveTexture;
	std::vector<float> emissiveFactor = {0.0f, 0.0f, 0.0f};
	std::string alphaMode = "OPAQUE";
	float alphaCutoff = 0.5f;
	bool doubleSided = false;

	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;
};