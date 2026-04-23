#pragma once
#include <GltfModel.hpp>
#include <MeshData.hpp>
#include <Transform.hpp>


struct LoadedModel {
	GltfModel gltf;
	std::vector<MeshData> meshes;
	Transform transform;
};