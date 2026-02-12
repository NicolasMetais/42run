#pragma once
#include <iostream>
#include <MeshData.hpp>
#include <GltfModel.hpp>
#include <utils.hpp>

class GltfImporter {
	public:
		MeshData buildMeshData(const GltfModel& model);
};