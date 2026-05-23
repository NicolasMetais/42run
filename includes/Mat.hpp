#pragma once
#include <iostream>
#include <json.hpp>
#include <Matrix/Matrix.hpp>
#include <Texture/Texture.hpp>

/** @brief Selects the shading model used by a material. */
enum MaterialType {
	PBR,   ///< Physically-based rendering (GLTF metallic-roughness workflow).
	PHONG  ///< Classic Phong shading (OBJ/MTL workflow).
};

/** @brief Reference to a texture slot within a material, including UV set and scale. */
struct TextureInfo {
	int index = -1;      ///< Index into the model's texture list (-1 = not set).
	int texCoord = 0;    ///< UV set index to use.
	float scale = 1.0f;
	float strength = 1.0f;
};

/** @brief PBR metallic-roughness parameters as defined by the GLTF spec. */
struct pbrMetallicRoughness {
	std::vector<float> baseColorFactor = {1.0f, 1.0f, 1.0f, 1.0f};
	TextureInfo baseColorTexture;
	float metallicFactor = -1;
	float roughnessFactor = -1;
	TextureInfo metallicRoughnessTexture;
};

/**
 * @brief Unified material descriptor supporting both PBR (GLTF) and Phong (OBJ) workflows.
 *
 * GPU texture pointers are resolved at load time so the renderer does not need to
 * walk the model's texture/image lists on every draw call.
 */
struct Mat {
	MaterialType type;
	std::string name;

	// GLTF PBR fields
	pbrMetallicRoughness MetallicRoughness;
	TextureInfo normalTexture;
	float normalTextureScale = 1.0f;
	TextureInfo occlusionTexture;
	float occlusionStrength = 1.0f;
	TextureInfo emissiveTexture;
	std::vector<float> emissiveFactor{0.0f, 0.0f, 0.0f};

	Texture* baseColorTextureGPU = nullptr;          ///< Resolved GPU texture for base color.
	Texture* metallicRoughnessTextureGPU = nullptr;  ///< Resolved GPU texture for metallic-roughness.
	Texture* normalTextureGPU = nullptr;
	Texture* occlusionTextureGPU = nullptr;
	Texture* emissiveTextureGPU = nullptr;

	std::string alphaMode = "OPAQUE"; ///< "OPAQUE", "MASK", or "BLEND".
	float alphaCutoff = 0.5f;
	bool doubleSided = false;

	// OBJ/MTL Phong fields
	Vector<float> Ka{0.1f, 0.1f, 0.1f}; ///< Ambient color.
	Vector<float> Kd{0.8f, 0.8f, 0.8f}; ///< Diffuse color.
	Vector<float> Ks{0.2f, 0.2f, 0.2f}; ///< Specular color.
	float Ns = 32;                        ///< Specular exponent.
	float Ni = 1.0f;                      ///< Index of refraction.
	float d = 1.0f;                       ///< Opacity (dissolve).
	int illum = 0;                        ///< Illumination model index.

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
