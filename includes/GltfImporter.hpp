#pragma once
#include <iostream>
#include <MeshData.hpp>
#include <Mesh.hpp>
#include <GltfModel.hpp>
#include <utils.hpp>

/** @brief Converts raw GLTF data into GPU-ready MeshData. */
class GltfImporter {
	public:
		/**
		 * @brief Builds a MeshData from a single GLTF mesh, resolving accessors and materials.
		 * @param model The fully parsed GLTF model containing all accessors and buffers.
		 * @param mesh  The mesh descriptor to convert.
		 * @return A MeshData with interleaved vertex data ready to be uploaded to the GPU.
		 */
		MeshData buildMeshData(const GltfModel& model, Mesh mesh);
};
