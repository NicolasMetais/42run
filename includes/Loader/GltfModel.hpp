#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <Loader/Mesh.hpp>
#include <json.hpp>
#include <Loader/Buffer.hpp>
#include <Loader/Node.hpp>
#include <Mat.hpp>
#include <Loader/Scene.hpp>
#include <Text.hpp>
#include <Loader/Image.hpp>
#include <Loader/GltfTypes.hpp>
#include <Texture/TextureSampler.hpp>
#include <Animation/Animation.hpp>
#include <Loader/Skin.hpp>
#include <Loader/AccessorView.hpp>

/** @brief Holds all data parsed from a GLTF/GLB file. */
struct GltfModel {
	std::vector<Buffer> buffers;
	std::vector<AccessorView> accessors;
	std::vector<BufferView> bufferViews;
	std::vector<Mesh> meshes;
	std::vector<Node> nodes;

	std::vector<Scene> scenes;
	int defaultScene = -1;
	std::vector<Mat> materials;
	std::vector<Text> textures;
	std::vector<Image> images;
	std::vector<TextureSampler> samplers;
	std::vector<Animation> animations;
	std::vector<Skin> skins;
	std::vector<uint8_t> binaryData; ///< Raw binary payload of a GLB file.

	/** @brief Parses all mesh primitives and their accessor references from the JSON. */
	void parseMeshes(const nlohmann::json& gltf);
	/** @brief Parses all buffer views (slices of buffers) from the JSON. */
	void parseBufferViews(const nlohmann::json& gltf);
	/** @brief Parses all accessor descriptors from the JSON. */
	void parseAccessors(const nlohmann::json& gltf);
	/** @brief Parses image source references from the JSON. */
	void parseImage(const nlohmann::json& gltf);
	/** @brief Parses texture sampler filter and wrap parameters from the JSON. */
	void parseTextureSampler(const nlohmann::json& gltf);
	/** @brief Parses texture descriptors (sampler + source pairs) from the JSON. */
	void parseTexture(const nlohmann::json& gltf);
	/** @brief Parses PBR material properties from the JSON. */
	void parseMaterials(const nlohmann::json& gltf);
	/** @brief Parses the scene graph nodes and their transform hierarchies from the JSON. */
	void parseNodes(const nlohmann::json& gltf);
	/** @brief Parses buffer URI references from the JSON. */
	void parseBuffer(const nlohmann::json& gltf);
	/** @brief Parses scene definitions and their root node lists from the JSON. */
	void parseScenes(const nlohmann::json& gltf);
	/** @brief Parses all animations (samplers and channels) from the JSON. */
	void parseAnimations(const nlohmann::json& gltf);
	/** @brief Parses skin definitions and inverse bind matrix accessors from the JSON. */
	void parseSkins(const nlohmann::json& gltf);

	/** @brief Prints a debug summary of all parsed data to stdout. */
	void printData() const;
	/** @brief Returns the total byte size of the elements described by accessor @p a. */
	size_t accesorByteSize(const AccessorView& a);
	/** @brief Validates that all buffer views reference valid byte ranges within their buffers. */
	void BufferViewValidation();

	GltfModel() = default;
	/** @brief Loads the raw binary chunk of a GLB file into binaryData. */
	void loadBinaryBuffer(const std::string& filename);
	/** @brief Parses the JSON chunk of a GLTF or GLB file and populates all data arrays. */
	void parseJson(std::string fileName);
};
