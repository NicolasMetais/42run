#include <GltfImporter.hpp>

MeshData GltfImporter::buildMeshData(const GltfModel& model, Mesh mesh) {
	const uint8_t* basePtr = model.binaryData.data();

    for (size_t primIdx = 0; primIdx < mesh.primitives.size(); ++primIdx) {
        const auto& prim = mesh.primitives[primIdx];

        // Accesseur de positions
        if (prim.positionAccessor < 0) continue;
        	const auto& posAccessor = model.accessors[prim.positionAccessor];

        // Accesseur de normales
        const AccessorView* normAccessor = nullptr;
        if (prim.normalAccessor >= 0)
            normAccessor = &model.accessors[prim.normalAccessor];

        for (size_t i = 0; i < posAccessor.count; ++i) {

            Vector<float> pos = posAccessor.getVector3(i, basePtr);
    	    Vector<float> norm = normAccessor ? normAccessor->getVector3(i, basePtr) : Vector<float>{0,0,0};
       	}
    }
	MeshData meshData;
	meshData.materials = model.materials; //je stock tout les materials
	meshData.images = model.images;
	meshData.textures = model.textures;

		//je vais voir dans chaque primitives
		for (const auto& primitive : mesh.primitives) {
			//je construit tout les submesh, un submesh par materials different
			SubMesh SubMesh;

			//materials
			if (primitive.materialIndex >= 0 && primitive.materialIndex < (int)model.materials.size())
				SubMesh.material = &meshData.materials[primitive.materialIndex];

			//Vertices
			if (primitive.positionAccessor < 0) continue;

			const auto& posAccessor = model.accessors[primitive.positionAccessor];
			SubMesh.hasPos = true;

			const AccessorView* normAccessor = nullptr;
			if (primitive.normalAccessor >= 0) {
				normAccessor = &model.accessors[primitive.normalAccessor];
				SubMesh.hasNormal = true;
			}

			const AccessorView* tangentAccessor = nullptr;
			if (primitive.tangentAccessor >= 0) {
				// std::cout << "tangent detecte GLTF IMPORTER" << std::endl;
				tangentAccessor = &model.accessors[primitive.tangentAccessor];
				SubMesh.hasTangent = true;
			}
			// const auto& normAccessor = model.accessors[primitive.normalAccessor];
			// const auto& tangentAccessor = model.accessors[primitive.tangentAccessor];

			//indices
			if (primitive.indexAccessor >= 0) {
				const auto& indexAccessor = model.accessors[primitive.indexAccessor];
				size_t indexCount = indexAccessor.count;

				SubMesh.indices.resize(indexCount);

				if ((int)indexAccessor.component == 5123) {
					SubMesh.indexType = GL_UNSIGNED_INT;
					for (size_t i = 0; i < indexCount; ++i) {
						SubMesh.indices[i] = indexAccessor.getScalar<uint16_t>(i, basePtr);
					}
				} else if ((int)indexAccessor.component == 5125) {
					SubMesh.indexType = GL_UNSIGNED_INT;
					for (size_t i = 0; i < indexCount; ++i) {
						SubMesh.indices[i] = indexAccessor.getScalar<uint32_t>(i, basePtr);
					}
				} else 
				throw std::runtime_error("Bad index type");
			} else
				SubMesh.indexType = GL_NONE;

			size_t vertexCount = posAccessor.count;
			SubMesh.vertices.resize(vertexCount);

			for (size_t i = 0; i < vertexCount; ++i) {
				auto& v = SubMesh.vertices[i];

				//pos
				if (posAccessor.type == ValueType::VEC3) {

					v.position = posAccessor.getVector3(i, basePtr);
				}
				else
					throw std::runtime_error("position can only be a vec3");
				
				//min et max calculation for bounding box and centering
				for (int k = 0; k < 3; ++k) {
					if (v.position[k] < meshData.min[k]) meshData.min[k] = v.position[k];
					if (v.position[k] > meshData.max[k]) meshData.max[k] = v.position[k];
				}
				//normal
				if (normAccessor && normAccessor->type == ValueType::VEC3) {
					v.normal = normAccessor->getVector3(i, basePtr);
					v.normal = v.normal.normalize();
					// std::cout << "normale :" << v.normal << std::endl;
				} else
					v.normal = {0.0f, 0.0f, 0.0f};
 
				//tangent
				if (tangentAccessor && tangentAccessor->type == ValueType::VEC4) {
					v.tangent = tangentAccessor->getVector4(i, basePtr);
				}
				else
					v.tangent = {0.0f, 0.0f, 0.0f, 0.0f};

				//uv
				if (!primitive.texcoords.empty()) {
					SubMesh.hasTexCoord = true;
					SubMesh.texCoordCount = primitive.texcoords.size();
				}
				for (auto& [setIndex, accessorIndex] : primitive.texcoords) {
					v.uv.resize(SubMesh.texCoordCount);
					v.uv[setIndex] = model.accessors[accessorIndex].type == ValueType::VEC2 ? model.accessors[accessorIndex].getVector2(i, basePtr) : Vector<float>{0,0};
				}
				//color
				if (!primitive.colors.empty()) {
					SubMesh.hasColor = true;
					SubMesh.colorCount = primitive.colors.size();
				}
				for (auto& [setIndex, accessorIndex] : primitive.colors) {
					Vector<float> c = model.accessors[accessorIndex].type == ValueType::VEC3 ? model.accessors[accessorIndex].getVector3(i, basePtr) : Vector<float>{1,1,1};
					v.color.clear();
					v.color.insert(v.color.end(), c.data.begin(), c.data.end()); //ignorance de l'alpha pour le moment
				}

				// joints, weights a faire en pluus sur le meme format que uv et color parce que hashmap
			}
			// Generate tangents from UV when the model doesn't provide them
			if (!SubMesh.hasTangent && SubMesh.hasTexCoord && !SubMesh.indices.empty()) {
				size_t vCount = SubMesh.vertices.size();
				std::vector<Vector<float>> tanT(vCount, Vector<float>{0.f, 0.f, 0.f});
				std::vector<Vector<float>> tanB(vCount, Vector<float>{0.f, 0.f, 0.f});

				for (size_t idx = 0; idx + 2 < SubMesh.indices.size(); idx += 3) {
					uint32_t i0 = SubMesh.indices[idx];
					uint32_t i1 = SubMesh.indices[idx + 1];
					uint32_t i2 = SubMesh.indices[idx + 2];
					if (i0 >= vCount || i1 >= vCount || i2 >= vCount) continue;

					const auto& v0 = SubMesh.vertices[i0];
					const auto& v1 = SubMesh.vertices[i1];
					const auto& v2 = SubMesh.vertices[i2];

					Vector<float> e1 = v1.position - v0.position;
					Vector<float> e2 = v2.position - v0.position;

					float du1 = v1.uv[0][0] - v0.uv[0][0];
					float dv1 = v1.uv[0][1] - v0.uv[0][1];
					float du2 = v2.uv[0][0] - v0.uv[0][0];
					float dv2 = v2.uv[0][1] - v0.uv[0][1];

					float denom = du1 * dv2 - du2 * dv1;
					if (std::abs(denom) < 1e-8f) continue;
					float r = 1.0f / denom;

					Vector<float> T{
						r * (dv2 * e1[0] - dv1 * e2[0]),
						r * (dv2 * e1[1] - dv1 * e2[1]),
						r * (dv2 * e1[2] - dv1 * e2[2])
					};
					Vector<float> B{
						r * (-du2 * e1[0] + du1 * e2[0]),
						r * (-du2 * e1[1] + du1 * e2[1]),
						r * (-du2 * e1[2] + du1 * e2[2])
					};

					tanT[i0] += T; tanT[i1] += T; tanT[i2] += T;
					tanB[i0] += B; tanB[i1] += B; tanB[i2] += B;
				}

				for (size_t i = 0; i < vCount; ++i) {
					auto& v = SubMesh.vertices[i];
					const Vector<float>& N = v.normal;
					Vector<float> T_orth = tanT[i] - N * N.dot(tanT[i]);
					if (T_orth.lengthSquared() < 1e-8f) {
						v.tangent = {1.f, 0.f, 0.f, 1.f};
						continue;
					}
					T_orth = T_orth.normalize();
					float w = (cross_product(N, T_orth).dot(tanB[i]) < 0.0f) ? -1.0f : 1.0f;
					v.tangent = {T_orth[0], T_orth[1], T_orth[2], w};
				}
				SubMesh.hasTangent = true;
			}

			meshData.submeshes.push_back(std::move(SubMesh));
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
	// utils::smoothNormals(meshData); //a applique QUE pour les .obj
	return meshData;
};
