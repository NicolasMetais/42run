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

constexpr uint32_t GltfModel::componentCount(ValueType t) {
	switch (t) {
		case ValueType::SCALAR: return 1;
        case ValueType::VEC2:   return 2;
        case ValueType::VEC3:   return 3;
        case ValueType::VEC4:   return 4;
        case ValueType::MAT2:   return 4;
        case ValueType::MAT3:   return 9;
        case ValueType::MAT4:   return 16;
    }
    return 0;
};

size_t GltfModel::componentSize(ComponentType c) {
	switch (c) {
		case ComponentType::BYTE:
		case ComponentType::UNSIGNED_BYTE: return 1;
		case ComponentType::SHORT:
		case ComponentType::UNSIGNED_SHORT: return 2;
		case ComponentType::UNSIGNED_INT:
		case ComponentType::FLOAT: return 4;
	}
	return 0;
}

size_t GltfModel::accesorByteSize(const AccessorView& a) {
	return a.count * componentCount(a.type) * componentSize(a.component);
}

void GltfModel::parseJson(std::string fileName) {
	nlohmann::json gltf;
	std::ifstream file(fileName);
	if (!file.is_open())
		throw std::runtime_error("Cannot open glTF file");
	file >> gltf;

	parseMeshes(gltf);
	parseBufferViews(gltf);
	parseAccessors(gltf);
	BufferViewValidation();

	parseImage(gltf);
	parseSampler(gltf);
	parseTexture(gltf);
	// a faire tout  a la fin: load du binaire + pas oublier de lier la base des bufferView au bon endroits du binaire
	loadBinaryBuffer("resources/" + gltf["buffers"][0]["uri"].get<std::string>());


};

void GltfModel::BufferViewValidation() {
	for (auto& accessor : accessors) {
		BufferView* bv = accessor.bufferView;

		size_t size = componentCount(accessor.type) * componentSize(accessor.component);

		size_t stride = (bv->byteStride != 0) ? bv->byteStride : size;

		if (bv->byteStride != 0 && bv->byteStride < size)
			throw std::runtime_error("BufferView byteStride is too small");
		if (accessor.byteOffset + stride * (accessor.count - 1) + size > bv->byteLength)
			throw std::runtime_error("Accessor in BufferView isn't compatible with byteLength bufferView");
	}
};

void GltfModel::parseImage(const nlohmann::json& gltf) {
	
}


void GltfModel::parseAccessors(const nlohmann::json& gltf) {
	if (!gltf.contains("accessors") || gltf["accessors"].empty())
		throw std::runtime_error("Gltf file has no accessors");
	for (auto& jsonAccessor : gltf["accessors"]) {
		AccessorView newAccessors;
		//BufferView
		if (!jsonAccessor.contains("bufferView"))
			throw std::runtime_error("There is no bufferView in one of the accessors");
		int idx = jsonAccessor["bufferView"].get<int>();
		if (idx < 0 || idx >= bufferViews.size())
			throw std::runtime_error("Accessor's bufferView index is corrupted");
		newAccessors.bufferView = &bufferViews[idx];

		//count
		if (!jsonAccessor.contains("count"))
			throw std::runtime_error("There is no count in one of the accessors");
		newAccessors.count = jsonAccessor["count"].get<uint32_t>();

		//componentType
		if (!jsonAccessor.contains("componentType"))
			throw std::runtime_error("There is no componentType in one of the accessors");
		uint16_t component = jsonAccessor["componentType"].get<uint16_t>();
		if (!((component >= 5120 && component <= 5123) || component == 5125 || component == 5126))
			throw std::runtime_error("Accesor's componentType is corrupted");
		newAccessors.component = static_cast<ComponentType>(component);

		//ValueType
		if (!jsonAccessor.contains("type")) 
			throw std::runtime_error("There is no type in one of the accessors");
		std::string type = jsonAccessor["type"].get<std::string>();
		if (type == "SCALAR")
			newAccessors.type = ValueType::SCALAR;
		else if (type == "VEC2")
			newAccessors.type = ValueType::VEC2;
		else if (type == "VEC3")
			newAccessors.type = ValueType::VEC3;
		else if (type == "VEC4")
			newAccessors.type = ValueType::VEC4;
		else if (type == "MAT2")
			newAccessors.type = ValueType::MAT2;
		else if (type == "MAT3")
			newAccessors.type = ValueType::MAT3;
		else if (type == "MAT4")
			newAccessors.type = ValueType::MAT4;
		else
			throw std::runtime_error("Accessor's type is corrupted");

		//ByteOffset
		if (jsonAccessor.contains("byteOffset"))
			newAccessors.byteOffset = jsonAccessor["byteOffset"].get<uint32_t>();
		
		//normalized
		if (jsonAccessor.contains("normalized"))
			newAccessors.normalized = jsonAccessor["normalized"].get<bool>();

		//name
		if (jsonAccessor.contains("name"))
			newAccessors.name = jsonAccessor["name"].get<std::string>();

		//extension
		if (jsonAccessor.contains("extensions")) {
			for (auto& [key, value] : jsonAccessor["extensions"].items())
				newAccessors.extensions[key] = value.get<std::string>();
		}

		//extras
		if (jsonAccessor.contains("extras")) {
			for (auto& [key, value] : jsonAccessor["extras"].items())
				newAccessors.extras[key] = value.get<std::string>();
		}

		accessors.push_back(newAccessors);
	}
}


void GltfModel::parseBufferViews(const nlohmann::json& gltf) {
	if (!gltf.contains("bufferViews") || gltf["bufferViews"].empty())
		throw std::runtime_error("Gltf file has no bufferViews");
	for (auto& jsonBufferViews : gltf["bufferViews"]) {
		BufferView newBufferView;

		//Buffer
		if (!jsonBufferViews.contains("buffer"))
			throw std::runtime_error("One of the bufferView has no buffer");
		uint32_t bufferIndex = jsonBufferViews["buffer"].get<uint32_t>();
		if (bufferIndex >= gltf["buffers"].size())
			throw std::runtime_error("bufferView buffer index out of raange");
		newBufferView.buffer = bufferIndex;

		//ByteLength
		if (!jsonBufferViews.contains("byteLength"))
			throw std::runtime_error("One of the bufferView has no byteLength");
		newBufferView.byteLength = jsonBufferViews["byteLength"].get<uint32_t>();
		if (newBufferView.byteLength == 0)
			throw std::runtime_error("bufferVew byteLength is zero");

		//ByteOffset
		if (jsonBufferViews.contains("byteOffset"))
			newBufferView.byteOffset = jsonBufferViews["byteOffset"].get<uint32_t>();

		//byteStride
		if (jsonBufferViews.contains("byteStride"))
			newBufferView.byteStride = jsonBufferViews["byteStride"].get<uint32_t>();

		//target
		if (jsonBufferViews.contains("target")) {
			uint32_t target = jsonBufferViews["target"].get<uint32_t>();
			if (target != 34962 && target != 34963)
				throw std::runtime_error("Invalid bufferView target");
			newBufferView.target = target;
		}

		//name
		if (jsonBufferViews.contains("name"))
			newBufferView.name = jsonBufferViews["name"].get<std::string>();
		
		//extensions
		if (jsonBufferViews.contains("extensions")) {
			for (auto& [key, value] : jsonBufferViews["extensions"].items())
				newBufferView.extensions[key] = value.get<std::string>();
		}

		//extras
		if (jsonBufferViews.contains("extras")) {
			for (auto& [key, value] : jsonBufferViews["extras"].items())
				newBufferView.extras[key] = value.get<std::string>();
		}

		bufferViews.push_back(newBufferView);
	}
};

void GltfModel::parseMeshes(const nlohmann::json& gltf) {
	if (!gltf.contains("meshes") || gltf["meshes"].empty())
		throw std::runtime_error("Gltf file has no meshes");
	
	for (auto& meshJson : gltf["meshes"]) {
		Mesh newMesh;
		if (!meshJson.contains("primitives") || meshJson["primitives"].empty())
			throw std::runtime_error("Mesh '" + meshJson.value("name", "Unnamed") + "' has no primitives");
		for (auto& primitives : meshJson["primitives"]) {
			Mesh::Primitive prim;

			if (!primitives.contains("attributes") || primitives["attributes"].empty())
				throw std::runtime_error("Primitives in mesh '" + meshJson.value("name", "Unnamed") + "' has no attributes");
			auto& attributes = primitives["attributes"];
			if (!attributes.contains("POSITION"))
				throw std::runtime_error("Primitives in mesh '" + meshJson.value("name", "Unnamed") + "' missing POSITION accessor");
			
			//Attributes
			for (auto& [key, value] : attributes.items()) {
				int accesorValue = value.get<int>();
				if (accesorValue < 0 || accesorValue >= gltf["accessors"].size())
					throw std::runtime_error("accessor out of bounds in primitive of mesh '" + meshJson.value("name", "Unnamed") + "'");
				if (key == "POSITION")
					prim.positionAccessor = accesorValue;
				else if (key == "NORMAL")
					prim.normalAccessor = accesorValue;
				else if (key == "TANGENT")
					prim.tangentAccessor= accesorValue;
				else if (key.find("TEXCOORD_") == 0) {
					int n = std::stoi(key.substr(9));
					prim.texcoords[n] = accesorValue;
				}
				else if (key.find("COLOR_") == 0) {
					int n = std::stoi(key.substr(6));
					prim.colors[n] = accesorValue;
				}
				else if (key.find("JOINTS_") == 0) {
					int n = std::stoi(key.substr(7));
					prim.joints[n] = accesorValue;
				}
				else if (key.find("WEIGHTS_") == 0) {
					int n = std::stoi(key.substr(8));
					prim.weights[n] = accesorValue;
				}
			}

			//indices
			if (primitives.contains("indices")) {
				int accessorVal = primitives["indices"].get<int>();
				if (accessorVal < 0 || accessorVal >= gltf["accessors"].size())
					throw std::runtime_error("accessor out of bounds in primitive of mesh '" + meshJson.value("name", "Unnamed") + "'");
				prim.indexAccessor = primitives["indices"].get<int>();
			}

			//material
			if (primitives.contains("material")) {
				int accessorVal = primitives["material"].get<int>();
				if (accessorVal < 0 || accessorVal >= gltf["materials"].size())
					throw std::runtime_error("accessor out of bounds in primitive of mesh '" + meshJson.value("name", "Unnamed") + "'");
				prim.materialIndex = primitives["material"].get<int>();
			}

			//morphTarget ici a faire later

			newMesh.primitives.push_back(prim);
		}

		meshes.push_back(newMesh);
	}
};

