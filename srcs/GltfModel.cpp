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
	parseMaterials(gltf);
	parseNodes(gltf);
	parseBuffer(gltf);
	loadBinaryBuffer("resources/" + gltf["buffers"][0]["uri"].get<std::string>());
	linkBufferViewToBinary();
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

void GltfModel::linkBufferViewToBinary() {
	if (binaryData.empty())
		throw std::runtime_error("BinaryData is empty");
	
	for (auto& bv : bufferViews) {
		Buffer& buf = buffers[bv.buffer];

		if (bv.byteOffset + bv.byteLength > buf.byteLength)
			throw std::runtime_error("BufferView exceeds buffer size");

		bv.base = binaryData.data() + bv.byteOffset;
	}
};

void GltfModel::parseBuffer(const nlohmann::json& gltf) {
	if (!gltf.contains("buffers") || gltf["buffers"].empty())
		throw std::runtime_error("There is no buffer");
	
	for (auto& jsonBuffer : gltf["buffers"]) {
		Buffer newBuffer;
		
		//uri
		if (!jsonBuffer.contains("uri"))
			throw std::runtime_error("uri is missing ! .glb not supported");
		newBuffer.uri = jsonBuffer["uri"].get<std::string>();

		//byteLength
		if (!jsonBuffer.contains("byteLength"))
			throw std::runtime_error("byteLength is mandatory in a buffer");
		uint64_t bytelength = jsonBuffer["byteLength"].get<uint64_t>();

		//open binary ton check the file size
		std::ifstream f( "resources/" + newBuffer.uri, std::ios::binary | std::ios::ate);
		if (!f.is_open())
			throw std::runtime_error("Cannot open buffer file");
		if ((int)bytelength > (int)f.tellg())
			throw std::runtime_error("Buffer byteLength larger than actual file size");
		if (bytelength < 0)
			throw std::runtime_error("bytelength has a ridiculous size");
		f.close();
		newBuffer.byteLength = bytelength;

		//name
		if (jsonBuffer.contains("name"))
			newBuffer.name = jsonBuffer["name"].get<std::string>();

		//extensions
		if (jsonBuffer.contains("extensions")) {
			for (auto& [key, value] : jsonBuffer["extensions"].items())
				newBuffer.extensions[key] = value;
		}

		//extras
		if (jsonBuffer.contains("extras")) {
			for (auto& [key, value] : jsonBuffer["extras"].items())
				newBuffer.extras[key] = value;
		}

		buffers.push_back(newBuffer);
	}

};


void GltfModel::parseNodes(const nlohmann::json& gltf) {
	if (!gltf.contains("nodes") || gltf["nodes"].empty())
		return ;
	
	for (auto& jsonNode : gltf["nodes"]) {
		Node newNode;

		//name
		if (jsonNode.contains("name"))
			newNode.name = jsonNode["name"].get<std::string>();

		//mesh
		if (jsonNode.contains("mesh")) {
			int mesh = jsonNode["mesh"].get<int>();
			if (mesh < 0 || mesh >= (int)gltf["meshes"].size())
				throw std::runtime_error("Corrupted mesh index in nodes");
			newNode.mesh = mesh;
		}

		//camera
		if(jsonNode.contains("camera")) {
			int camera = jsonNode["cameras"].get<int>();
			if (camera < 0 || camera >= (int)gltf["camera"].size())
				throw std::runtime_error("Corrupted camera index in nodes");
			newNode.camera = camera;
		}

		//children
		if (jsonNode.contains("children")){
			std::vector<int> children = jsonNode["children"].get<std::vector<int>>();
			for (int i : children){
				if (i < 0 || i >= int(gltf["nodes"].size()))
					throw std::runtime_error("invalid child node index");
			}
			newNode.children = children;
		}

		//translation
		if (jsonNode.contains("translation") && !jsonNode.contains("matrix")) {
			std::vector<float> translation = jsonNode["translation"].get<std::vector<float>>();
			if (translation.size() != 3)
				throw std::runtime_error("Wrong translation vector size");
			newNode.translation = translation;
		}

		//rotation
		if (jsonNode.contains("rotation") && !jsonNode.contains("matrix")) {
			std::vector<float> rotation = jsonNode["rotation"].get<std::vector<float>>();
			if (rotation.size() != 4)
				throw std::runtime_error("Wrong rotation vector size");
			newNode.rotation = rotation;
		}

		//scale
		if (jsonNode.contains("scale") && !jsonNode.contains("matrix")) {
			std::vector<float> scale = jsonNode["scale"].get<std::vector<float>>();
			if (scale.size() != 3)
				throw std::runtime_error("Wrong scale vector size");
			newNode.scale = scale;
		}

		//matrix
		if (jsonNode.contains("matrix")) {
			std::vector<float> matrix = jsonNode["matrix"].get<std::vector<float>>();
			if (matrix.size() != 16) //attention faut verifier un vrai 4x4
				throw std::runtime_error("Wrong matrix size");
			newNode.matrix = matrix;
		}

		//extensions
		if (jsonNode.contains("extensions")) {
			for (auto& [key, value] : jsonNode["extensions"].items())
				newNode.extensions[key] = value;
		}

		//extras
		if (jsonNode.contains("extras")) {
			for (auto& [key, value] : jsonNode["extras"].items())
				newNode.extras[key] = value;
		}

		nodes.push_back(newNode);
	}
};

static TextureInfo parseTextureInfo(const nlohmann::json& obj, const nlohmann::json& gltf) {
	TextureInfo t;

	if (!obj.contains("index"))
		throw std::runtime_error("textureInfo missing index");

	t.index = obj["index"].get<int>();
	if (t.index < 0 || t.index >= (int)gltf["textures"].size())
		throw std::runtime_error("texture index out of range");
	
	if (obj.contains("texCoord")) {
		t.texCoord = obj["texCoord"].get<int>();
		if (t.texCoord < 0)
			throw std::runtime_error("invalid texCoord");
	}

	if (obj.contains("scale"))
		t.scale = obj["scale"].get<float>();

	if (obj.contains("strength"))
		t.strength = obj["strength"].get<float>();

	return t;
}


void GltfModel::parseMaterials(const nlohmann::json& gltf) {
	if (!gltf.contains("materials") || gltf["materials"].empty())
		return; //Y A UN TRUC CHELOU A GERER SI Y A PAS DE MATERIALS

	for (auto& jsonMaterial : gltf["materials"]) {
		Mat newMaterial;

		//pbrMetallicRougness
		if (jsonMaterial.contains("pbrMetallicRoughness")) {
			auto& pbr = jsonMaterial["pbrMetallicRoughness"];

			//baseColorTexture
			if (pbr.contains("baseColorTexture"))
				newMaterial.pbrMetallicRoughness.baseColorTexture = parseTextureInfo(pbr["baseColorTexture"], gltf);

			//baseColorFactor
			if (pbr.contains("baseColorFactor")) {
				newMaterial.pbrMetallicRoughness.baseColorFactor =pbr["baseColorFactor"].get<std::vector<float>>();
			}

			//metallicFactor
			if (pbr.contains("metallicFactor")) {
				float metallicFactor = pbr["metallicFactor"].get<float>();
				if (metallicFactor < 0 || metallicFactor > 1)
					throw std::runtime_error("MetallicFactor corrupted");
				newMaterial.pbrMetallicRoughness.metallicFactor = metallicFactor;
			}

			//roughnessFactor
			if (pbr.contains("roughnessFactor")) {
				float roughnessFactor = pbr["roughnessFactor"].get<float>();
				if (roughnessFactor < 0 || roughnessFactor > 1)
					throw std::runtime_error("roughnessFactor corrupted");
				newMaterial.pbrMetallicRoughness.roughnessFactor = roughnessFactor;
			}

			//metallicRoughnessTexture
			if (pbr.contains("metallicRoughnessTexture"))
				newMaterial.pbrMetallicRoughness.metallicRoughnessTexture = parseTextureInfo(pbr["metallicRoughnessTexture"], gltf);
		}

		//name
		if (jsonMaterial.contains("name"))
			newMaterial.name = jsonMaterial["name"].get<std::string>();
		
		//normalTexture
		if (jsonMaterial.contains("normalTexture"))
			newMaterial.normalTexture = parseTextureInfo(jsonMaterial["normalTexture"], gltf);

		//normalTextureScale
		if (jsonMaterial.contains("normalTextureScale")) {
			float normalTextureScale = jsonMaterial["normalTextureScale"].get<float>();
			if (normalTextureScale < 0)
				throw std::runtime_error("negativ textureScale");
			newMaterial.normalTextureScale = normalTextureScale;
		}

		//occlusionTexture
		if (jsonMaterial.contains("occlusionTexture"))
			newMaterial.occlusionTexture = parseTextureInfo(jsonMaterial["occlusionTexture"], gltf);

		//occlusionStrength
		if (jsonMaterial.contains("occlusionStrength")) {
			float occlusionStrength = jsonMaterial["occlusionStrength"].get<float>();
			if (occlusionStrength < 0.0f || occlusionStrength > 1.0f)
				throw std::runtime_error("negative occlusionStrength");
			newMaterial.occlusionStrength = occlusionStrength;
		}

		//emissiveTexture
		if (jsonMaterial.contains("emissiveTexture"))
			newMaterial.emissiveTexture = parseTextureInfo(jsonMaterial["emissiveTexture"], gltf);

		//emissiveFactor
		if (jsonMaterial.contains("emissiveFactor")) {
			std::vector<float> emissiveFactor = jsonMaterial["emissiveFactor"].get<std::vector<float>>();
			if (emissiveFactor.size() != 3)
				throw std::runtime_error("Wrong emissiveFactor vector size");
			for (auto& i : emissiveFactor) {
					if (i < 0 || i > 1.0)
						throw std::runtime_error("corrupted emissiveFactor vector");
				}
			newMaterial.emissiveFactor = emissiveFactor;
		}

		//alphaMode
		if (jsonMaterial.contains("alphaMode")) {
			std::string alphaMode = jsonMaterial["alphaMode"].get<std::string>();
			if (alphaMode != "OPAQUE" && alphaMode != "MASK" && alphaMode != "BLEND")
				throw std::runtime_error("invalid alphaMode value");
			newMaterial.alphaMode = alphaMode;
		}

		//alphaCutoff
		if (jsonMaterial.contains("alphaCutoff")) {
			float alphaCutoff = jsonMaterial["alphaCutoff"].get<float>();
			if (alphaCutoff < 0.0f || alphaCutoff > 1.0f)
				throw std::runtime_error("Corrupted alphaCutoff");
			newMaterial.alphaCutoff = alphaCutoff;
		}

		//doubleSided
		if (jsonMaterial.contains("doubleSided"))
			newMaterial.doubleSided = jsonMaterial["doubleSided"].get<bool>();

		//extensions
		if (jsonMaterial.contains("extensions")) {
			for (auto& [key, value] : jsonMaterial["extensions"].items())
				newMaterial.extensions[key] = value;
		}

		//extras
		if (jsonMaterial.contains("extras")) {
			for (auto& [key, value] : jsonMaterial["extras"].items())
				newMaterial.extras[key] = value;
		}

		materials.push_back(newMaterial);
	}
};


void GltfModel::parseSampler(const nlohmann::json& gltf) {
	if (!gltf.contains("samplers") || gltf["samplers"].empty())
		return ;
	
	for (auto& jsonSampler : gltf["samplers"]) {
		Sampler newSampler;

		//magfilter
		if (jsonSampler.contains("magFilter")) {
			int magFilter = jsonSampler["magFilter"].get<int>();
			if (magFilter != 9728 && magFilter != 9729)
				throw std::runtime_error("Magfilter corrupted");
			newSampler.magFilter = magFilter;
		}

		//magfilter
		if (jsonSampler.contains("minFilter")) {
			int minFilter = jsonSampler["minFilter"].get<int>();
			if (minFilter != 9728 && minFilter != 9729 && (minFilter < 9984 || minFilter > 9987))
				throw std::runtime_error("MinFilter corrupted");
			newSampler.minFilter = minFilter;
		}

		//magfilter
		if (jsonSampler.contains("wrapS")) {
			int wrapS = jsonSampler["wrapS"].get<int>();
			if (wrapS != 33071 && wrapS != 33648 && wrapS != 10497)
				throw std::runtime_error("wrapS corrupted");
			newSampler.wrapS = wrapS;
		}

		//magfilter
		if (jsonSampler.contains("wrapT")) {
			int wrapT = jsonSampler["wrapT"].get<int>();
			if (wrapT != 33071 && wrapT != 33648 && wrapT != 10497)
				throw std::runtime_error("wrapT corrupted");
			newSampler.wrapT = wrapT;
		}
		
		//name
		if (jsonSampler.contains("name"))
			newSampler.name = jsonSampler["name"].get<std::string>();

		//extensions
		if (jsonSampler.contains("extensions")) {
			for (auto& [key, value] : jsonSampler["extensions"].items())
				newSampler.extensions[key] = value;
		}

		//extras
		if (jsonSampler.contains("extras")) {
			for (auto& [key, value] : jsonSampler["extras"].items())
				newSampler.extras[key] = value;
		}
		samplers.push_back(newSampler);
	}
};

void GltfModel::parseTexture(const nlohmann::json& gltf) {
	if (!gltf.contains("textures") || gltf["textures"].empty())
		return ;

	for (auto& jsonTexture : gltf["textures"]) {
		Text newTexture;

		//sampler
		if (jsonTexture.contains("sampler")) {
			int sampler = jsonTexture["sampler"].get<int>();
			if (sampler < 0 || sampler >= (int)gltf["samplers"].size())
				throw std::runtime_error("One of the samplers values is corrupted");
			newTexture.sampler = sampler;
		}

		//source
		if (!jsonTexture.contains("source"))
			throw std::runtime_error("Texture has no source image");
		int source = jsonTexture["source"].get<int>();
		if (source < 0 || source >= (int)gltf["images"].size())
			throw std::runtime_error("One of the source values is corrupted");
		newTexture.source = source;

		//name
		if (jsonTexture.contains("name"))
			newTexture.name = jsonTexture["name"].get<std::string>();

		//extensions
		if (jsonTexture.contains("extensions")) {
			for (auto& [key, value] : jsonTexture["extensions"].items())
				newTexture.extensions[key] = value;
		}

		//extras
		if (jsonTexture.contains("extras")) {
			for (auto& [key, value] : jsonTexture["extras"].items())
				newTexture.extras[key] = value;
		}

		textures.push_back(newTexture);
	}
};

void GltfModel::parseImage(const nlohmann::json& gltf) {
	if (!gltf.contains("images") || gltf["images"].empty())
		return ;
	
	for (auto& jsonImage : gltf["images"]) {
		Image newImage;

		bool hasUri = jsonImage.contains("uri");
		bool hasBufferView = jsonImage.contains("bufferView");

		if (hasUri == hasBufferView)
			throw std::runtime_error("Image must have either uri or bufferView (but not both)");

		//uri
		if (hasUri)
			newImage.uri = jsonImage["uri"].get<std::string>();

		//bufferView
		if (hasBufferView) {
			if (!jsonImage.contains("mimeType"))
				throw std::runtime_error("mimeType is mandatory when bufferView is present");
			int bv = jsonImage["bufferView"].get<int>();
			if (bv < 0 || bv >= (int)bufferViews.size())
				throw std::runtime_error("BufferView out of bound index");
			newImage.bufferView = bv;

			//mimeType
			newImage.mimeType = jsonImage["mimeType"].get<std::string>();

		}
		
		//name
		if (jsonImage.contains("name"))
			newImage.name = jsonImage["name"].get<std::string>();

		//extensions
		if (jsonImage.contains("extensions")) {
			for (auto& [key, value] : jsonImage["extensions"].items())
				newImage.extensions[key] = value;
		}

		//extras
		if (jsonImage.contains("extras")) {
			for (auto& [key, value] : jsonImage["extras"].items())
				newImage.extras[key] = value;
		}

		images.push_back(newImage);
	}
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
		if (idx < 0 || idx >= (int)bufferViews.size())
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
				newAccessors.extensions[key] = value;
		}

		//extras
		if (jsonAccessor.contains("extras")) {
			for (auto& [key, value] : jsonAccessor["extras"].items())
				newAccessors.extras[key] = value;
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
				newBufferView.extensions[key] = value;
		}

		//extras
		if (jsonBufferViews.contains("extras")) {
			for (auto& [key, value] : jsonBufferViews["extras"].items())
				newBufferView.extras[key] = value;
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
				if (accesorValue < 0 || accesorValue >= (int)gltf["accessors"].size())
					throw std::runtime_error("accessor out of bounds in primitive of mesh '" + meshJson.value("name", "Unnamed") + "'");
				if (key == "POSITION")
					prim.positionAccessor = accesorValue;
				else if (key == "NORMAL") {
					prim.normalAccessor = accesorValue;
				}
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
				if (accessorVal < 0 || accessorVal >= (int)gltf["accessors"].size())
					throw std::runtime_error("accessor out of bounds in primitive of mesh '" + meshJson.value("name", "Unnamed") + "'");
				prim.indexAccessor = primitives["indices"].get<int>();
			}

			//material
			if (primitives.contains("material")) {
				int accessorVal = primitives["material"].get<int>();
				if (accessorVal < 0 || accessorVal >= (int)gltf["materials"].size())
					throw std::runtime_error("accessor out of bounds in primitive of mesh '" + meshJson.value("name", "Unnamed") + "'");
				prim.materialIndex = primitives["material"].get<int>();
			}

			//morphTarget ici a faire later

			newMesh.primitives.push_back(prim);
		}

		meshes.push_back(newMesh);
	}
};

void GltfModel::printData() const {
    std::cout << "=== Buffers ===\n";
    for (size_t i = 0; i < buffers.size(); ++i) {
        std::cout << "Buffer[" << i << "]: uri=" << buffers[i].uri 
                  << ", byteLength=" << buffers[i].byteLength << "\n";
    }

    std::cout << "\n=== BufferViews ===\n";
    for (size_t i = 0; i < bufferViews.size(); ++i) {
        std::cout << "BufferView[" << i << "]: buffer=" << bufferViews[i].buffer
                  << ", offset=" << bufferViews[i].byteOffset
                  << ", length=" << bufferViews[i].byteLength
                  << ", stride=" << bufferViews[i].byteStride << "\n";
    }

    std::cout << "\n=== Accessors ===\n";
    for (size_t i = 0; i < accessors.size(); ++i) {
        std::cout << "Accessor[" << i << "]: bufferView=" << (accessors[i].bufferView ? std::to_string(accessors[i].bufferView - &bufferViews[0]) : "null")
                  << ", count=" << accessors[i].count
                  << ", type=" << static_cast<int>(accessors[i].type)
                  << ", component=" << static_cast<int>(accessors[i].component)
                  << ", byteOffset=" << accessors[i].byteOffset
                  << "\n";
    }

    std::cout << "\n=== Meshes ===\n";
    for (size_t i = 0; i < meshes.size(); ++i) {
        std::cout << "Mesh[" << i << "]: " << meshes[i].primitives.size() << " primitives\n";
        for (size_t j = 0; j < meshes[i].primitives.size(); ++j) {
            const auto& prim = meshes[i].primitives[j];
            std::cout << "  Primitive[" << j << "]: posAccessor=" << prim.positionAccessor
                      << ", normalAccessor=" << prim.normalAccessor
                      << ", tangentAccessor=" << prim.tangentAccessor
                      << ", indicesAccessor=" << prim.indexAccessor
                      << ", materialIndex=" << prim.materialIndex
                      << ", texcoords=" << prim.texcoords.size() << "\n";
        }
    }

    std::cout << "\n=== Materials ===\n";
    for (size_t i = 0; i < materials.size(); ++i) {
        std::cout << "Material[" << i << "]: name=" << materials[i].name
                  << ", baseColorTexture=" << (materials[i].pbrMetallicRoughness.baseColorTexture.index)
                  << ", metallicRoughnessTexture=" << (materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index)
                  << ", normalTexture=" << (materials[i].normalTexture.index)
                  << ", emissiveTexture=" << (materials[i].emissiveTexture.index)
                  << ", occlusionTexture=" << (materials[i].occlusionTexture.index)
                  << "\n";
    }

    std::cout << "\n=== Nodes ===\n";
    for (size_t i = 0; i < nodes.size(); ++i) {
        std::cout << "Node[" << i << "]: name=" << nodes[i].name
                  << ", mesh=" << nodes[i].mesh
                  << ", children=" << nodes[i].children.size() << "\n";
    }
};


