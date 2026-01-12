#pragma once
#include <iostream>
#include <fstream>

#include <vector>
#include <AccessorView.hpp>
#include <Mesh.hpp>
#include <json.hpp>
#include <Buffer.hpp>
#include <Node.hpp>
#include <Mat.hpp>
#include <Scene.hpp>
#include <Text.hpp>
#include <Image.hpp>
#include <Sampler.hpp>
#include <Animation.hpp>
#include <Skin.hpp>


class GltfModel {
	private:
		std::vector<Buffer> buffers;
		std::vector<AccessorView> accessors;
		std::vector<BufferView> bufferViews;

		std::vector<Mesh> meshes;
		std::vector<Node> nodes;
		std::vector<Scene> scenes;

		std::vector<Mat> materials;
		std::vector<Text> textures;
		std::vector<Image> images;
		std::vector<Sampler> samplers;

		std::vector<Animation> animations;
		std::vector<Skin> skins;

		std::vector<uint8_t> binaryData;
		void parseMeshes(const nlohmann::json& gltf);
		void parseBufferViews(const nlohmann::json& gltf);
		void parseAccessors(const nlohmann::json& gltf);
		void parseImage(const nlohmann::json& gltf);
		void parseSampler(const nlohmann::json& gltf);
		void parseTexture(const nlohmann::json& gltf);
		void parseMaterials(const nlohmann::json& gltf);
		void parseNodes(const nlohmann::json& gltf);
		void parseScenes(const nlohmann::json& gltf);
		constexpr uint32_t componentCount(ValueType t);
		size_t accesorByteSize(const AccessorView& a);
		size_t componentSize(ComponentType c);
		void 	BufferViewValidation();
	public:
		GltfModel() = default;
		void loadBinaryBuffer(const std::string& filename);
		void parseJson(std::string fileName);
};