#pragma once
#include <iostream>
#include <vector>
#include <Mesh.hpp>
#include <json.hpp>

class GltfModel {
	private:
		std::vector<Mesh> meshList;
		std::vector<uint8_t> binaryData;
	public:
		GltfModel() {};
		void loadBinaryBuffer(const std::string& filename);
		void parseJson(std::string fileName);
};