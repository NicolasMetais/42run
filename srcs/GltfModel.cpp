#include <GltfModel.hpp>

void GltfModel::loadBinaryBuffer(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (!file)
		throw std::runtime_error("Cannot open binary file " + filename);
	std::streamsize size = file.tellg();
	if (size <= 0)
		throw std::runtime_error("Binary file is empty" + filename);
	file.seekg(0, std::ios::beg);
	binaryData.resize(size);
	if (!file.read(reinterpret_cast<char *>(binaryData.data()), size))
		throw std::runtime_error("Read binary file failed");
}

void GltfModel::parseJson(std::string fileName) {
	nlohmann::json gltf;
	std::ifstream file(fileName);
	if (!file.is_open())
		throw std::runtime_error("Cannot open glTF file");
	file >> gltf;
	loadBinaryBuffer("resources/" + gltf["buffers"][0]["uri"].get<std::string>());

	size_t meshNum = gltf["meshes"].size();
	this->meshList.resize(meshNum);

	for (size_t i = 0; i < meshNum; ++i) {
		auto& meshJson = gltf["meshes"][i];
		this->meshList[i].setName(meshJson["name"].get<std::string>());

		for (auto& primitives : meshJson["primitives"]) {
			(void)primitives;
		}
	}
};
