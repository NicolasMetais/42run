#include <GltfImporter.hpp>

MeshData GltfImporter::buildMeshData(const GltfModel& model) {
	for (size_t meshIdx = 0; meshIdx < model.meshes.size(); ++meshIdx) {
    const auto& mesh = model.meshes[meshIdx];
    std::cout << "Mesh #" << meshIdx << ":\n";

    for (size_t primIdx = 0; primIdx < mesh.primitives.size(); ++primIdx) {
        const auto& prim = mesh.primitives[primIdx];
        std::cout << "  Primitive #" << primIdx << ":\n";

        // Accesseur de positions
        if (prim.positionAccessor < 0) continue;
        const auto& posAccessor = model.accessors[prim.positionAccessor];

        // Accesseur de normales
        const AccessorView* normAccessor = nullptr;
        if (prim.normalAccessor >= 0)
            normAccessor = &model.accessors[prim.normalAccessor];

        for (size_t i = 0; i < posAccessor.count; ++i) {
            Vector<float> pos = posAccessor.getVector3(i);
            Vector<float> norm = normAccessor ? normAccessor->getVector3(i) : Vector<float>{0,0,0};

            std::cout << "    Vertex[" << i << "] ";
            std::cout << "pos: (" << pos[0] << ", " << pos[1] << ", " << pos[2] << ") ";
            std::cout << "norm: (" << norm[0] << ", " << norm[1] << ", " << norm[2] << ")\n";
        }
    }
}
	MeshData meshData;
	meshData.materials = model.materials; //je stock tout les materials
	meshData.images = model.images;
	meshData.textures = model.textures;

	//je me deplace dans tout les meshes
	for (const auto& mesh : model.meshes) {
		//je vais voir dans chaque primitives
		for (const auto& primitive : mesh.primitives) {
			//je construit tout les submesh, un submesh par materials different
			SubMesh SubMesh;

			//materials
			if (primitive.materialIndex >= 0 && primitive.materialIndex < (int)model.materials.size())
				SubMesh.material = &model.materials[primitive.materialIndex];

			//Vertices
			if (primitive.positionAccessor < 0) continue;

			const auto& posAccessor = model.accessors[primitive.positionAccessor];

			const AccessorView* normAccessor = nullptr;
			if (primitive.normalAccessor >= 0)
				normAccessor = &model.accessors[primitive.normalAccessor];

			const AccessorView* tangentAccessor = nullptr;
			if (primitive.tangentAccessor >= 0)
				tangentAccessor = &model.accessors[primitive.tangentAccessor];
			// const auto& normAccessor = model.accessors[primitive.normalAccessor];
			// const auto& tangentAccessor = model.accessors[primitive.tangentAccessor];

			//indices
			if (primitive.indexAccessor >= 0) {
				const auto& indexAccessor = model.accessors[primitive.indexAccessor];
				size_t indexCount = indexAccessor.count;

				SubMesh.indices.resize(indexCount);
				for (size_t i = 0; i < indexCount; ++i) {
					SubMesh.indices[i] = indexAccessor.getScalar<uint16_t>(i);
				}
			}

			size_t vertexCount = posAccessor.count;
			SubMesh.vertices.resize(vertexCount);

			for (size_t i = 0; i < vertexCount; ++i) {
				auto& v = SubMesh.vertices[i];

				//pos
				if (posAccessor.type == ValueType::VEC3)
					v.position = posAccessor.getVector3(i);
				else
					throw std::runtime_error("position can only be a vec3");
				
				//min et max calculation for bounding box and centering
				for (int k = 0; k < 3; ++k) {
					if (v.position[k] < meshData.min[k]) meshData.min[k] = v.position[k];
					if (v.position[k] > meshData.max[k]) meshData.max[k] = v.position[k];
				}
				//normal
				if (normAccessor && normAccessor->type == ValueType::VEC3) {
						v.normal = normAccessor->getVector3(i);
					v.normal = v.normal.normalize();
					// std::cout << "normal: ("
					// << v.normal[0] << ", "
					// << v.normal[1] << ", "
					// << v.normal[2] << ")\n";
				} else
					v.normal = {0.0f, 0.0f, 0.0f};
 
				//tangent
				if (tangentAccessor && tangentAccessor->type == ValueType::VEC3)
					v.tangent = tangentAccessor->getVector3(i);
				else
					v.tangent = {0.0f, 0.0f, 0.0f}; //a gerer

				//uv
				v.uv.resize(primitive.texcoords.size());
				size_t uvIndex = 0;
				for (auto& [key, index] : primitive.texcoords)
					v.uv[uvIndex++] = model.accessors[index].type == ValueType::VEC2 ? model.accessors[index].getVector2(i) : Vector<float>{0,0};

				//color
				v.color.clear();
				for (auto& [key, index] : primitive.colors) {
					Vector<float> c = model.accessors[index].type == ValueType::VEC3 ? model.accessors[index].getVector3(i) : Vector<float>{1,1,1};
					v.color.insert(v.color.end(), c.data.begin(), c.data.end()); //ignorance de l'alpha pour le moment
				}

				// joints, weights a faire en pluus
			}
			meshData.submeshes.push_back(std::move(SubMesh));
		}
	}
	meshData.center = (meshData.min + meshData.max) * 0.5f;
	float dist = 0.0f;
	for (const auto& sub : meshData.submeshes) {
		for (const auto& v : sub.vertices) {		
			float rad = (v.position - meshData.center).lengthSquared();
			dist = std::max(dist, rad);
		}
	}
	meshData.radius = sqrt(dist);
	utils::smoothNormals(meshData);
	return meshData;
};
