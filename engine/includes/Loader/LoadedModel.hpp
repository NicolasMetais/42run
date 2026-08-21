#pragma once
#include <Loader/GltfModel.hpp>
#include <MeshData.hpp>
#include <Scene/Transform.hpp>

/** @brief A fully loaded model: parsed GLTF data, GPU-ready meshes, world transform, and joint matrices. */
struct LoadedModel {
	GltfModel gltf;                                    ///< Parsed GLTF asset (nodes, skins, animations…).
	std::vector<MeshData> meshes;                      ///< GPU-ready mesh data, one entry per GLTF mesh.
	Transform transform;                               ///< World-space position, rotation and scale.
	std::vector<std::vector<Matrix<float>>> jointMatrices; ///< Per-skin list of joint matrices for skeletal animation.
};
