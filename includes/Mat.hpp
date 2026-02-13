#pragma once
#include <iostream>
#include <json.hpp>
#include <Matrix/Matrix.hpp>
#include <Texture.hpp>


enum MaterialType {
	PBR,
	PHONG
};

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
	MaterialType type;
	std::string name;

	//GLTF
	pbrMetallicRoughness pbrMetallicRoughness;
	TextureInfo normalTexture;
	float normalTextureScale = 1.0f;
	TextureInfo occlusionTexture;
	float occlusionStrength = 1.0f;
	TextureInfo emissiveTexture;
	std::vector<float> emissiveFactor{0.0f, 0.0f, 0.0f};

	// pointeur gpu directement sur les bonne textures,
	// pour pas avoir a tout reparcourir pour trouver les bonne data de textures

	Texture* baseColorTextureGPU = nullptr;
	Texture* metallicRoughnessTextureGPU = nullptr;
	Texture* normalTextureGPU = nullptr;
	Texture* occlusionTextureGPU = nullptr;
	Texture* emissiveTextureGPU = nullptr;

	std::string alphaMode = "OPAQUE";
	float alphaCutoff = 0.5f;
	bool doubleSided = false;

	//OBJ
	Vector<float> Ka{0.1f, 0.1f, 0.1f};
	Vector<float> Kd{0.8f, 0.8f, 0.8f};
	Vector<float> Ks{0.2f, 0.2f, 0.2f};
	float Ns = 32;
	float Ni = 1.0f;
	float d = 1.0f;
	int illum = 0;

	std::string map_Ka;
	std::string map_Ks;
	std::string map_Kd;
	std::string map_Ns;
	std::string map_d;
	std::string bump;

	Texture* map_KaGPU = nullptr;
	Texture* map_KsGPU = nullptr;
	Texture* map_KdGPU = nullptr;
	Texture* map_NsGPU = nullptr;
	Texture* map_dGPU = nullptr;
	Texture* bumpGPU = nullptr;

	std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;
};