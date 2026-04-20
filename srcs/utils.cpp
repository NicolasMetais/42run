#include <utils.hpp>

Matrix<float> utils::rotationX(float x) {
		float c = cosf(x);
		float s = sinf(x);
		return Matrix<float> ({
			{1,0,0,0},
			{0,c,-s,0},
			{0,s,c,0},
			{0,0,0,1}}
		);
};

Matrix<float> utils::rotationY(float y) {
		float c = cosf(y);
		float s = sinf(y);
		return Matrix<float>({
			{c,0,s,0},
			{0,1,0,0},
			{-s,0,c,0},
			{0,0,0,1}}
		);
};

Matrix<float> utils::rotationZ(float z) {
		float c = cosf(z);
		float s = sinf(z);
		return Matrix<float>({
			{c,-s,0,0},
			{s,c,0,0},
			{0,0,1,0},
			{0,0,0,1}}
		);
};

Matrix<float> utils::perspective(float fov, float aspect, float near, float far) {
		float scale = 1.0f / tan(fov / 2.0f);
		return Matrix<float>({
			{scale/aspect,0,0,0},
			{0,scale,0,0},
			{0,0,(far + near) / (near - far), (2 * far * near) / (near - far)},
			{0,0,-1,0}}
		);
};

Matrix<float> utils::view(Vector<float> pos, Vector<float> center, Vector<float> up) {
    Vector<float> f = (center - pos).normalize();
    Vector<float> r = cross_product(f, up).normalize(); 
    Vector<float> u = cross_product(r, f);

    Matrix<float> view({
        {r.x(), r.y(), r.z(), -r.dot(pos)},
        {u.x(), u.y(), u.z(), -u.dot(pos)},
        {-f.x(), -f.y(), -f.z(), f.dot(pos)},
        {0.0f, 0.0f, 0.0f, 1.0f}
    });
    
    return view;
}

Matrix<float> utils::translation(const Vector<float>& position) {
		Matrix<float> translation = identity<float>(4);
		translation[0][3] = position.x();
		translation[1][3] = position.y();
		translation[2][3] = position.z();
		return translation;
};

float utils::Todegres(float rad) {
		return rad * (180.0f / M_PI);
	};

float utils::ToRad(float degres) {
		return degres * (M_PI / 180.0f);
	};

Vector<float> computeFaceNormal(const Vector<float>& p0, const Vector<float>& p1, const Vector<float>& p2) {
	Vector<float> edge1 = p1 - p0;
	Vector<float> edge2 = p2 - p0;
	return cross_product(edge1, edge2);
}

static inline std::size_t hash_tuple(int a, int b, int c) {
    return std::hash<long long>()(((long long)a << 42) ^ ((long long)b << 21) ^ (long long)c);
}

void utils::smoothNormals(MeshData& meshdata)
{
    const float eps = 1e-6f;

	for (auto& submesh : meshdata.submeshes) {

		std::vector<bool> usedVertices(submesh.vertices.size(), false);
		for (size_t idx : submesh.indices)
			if (idx < submesh.vertices.size())
				usedVertices[idx] = true;

		std::unordered_map<std::size_t, std::vector<size_t>> posGroups;
		posGroups.reserve(submesh.vertices.size() * 2);

		for (size_t i = 0; i < submesh.vertices.size(); ++i) {
			if (!usedVertices[i]) continue;
			auto& v = submesh.vertices[i];
			int xi = static_cast<int>(std::floor(v.position[0] / eps));
			int yi = static_cast<int>(std::floor(v.position[1] / eps));
			int zi = static_cast<int>(std::floor(v.position[2] / eps));
			std::size_t h = hash_tuple(xi, yi, zi);
			posGroups[h].push_back((int)i);
		}

    	std::vector<Vector<float>> smoothNormals(submesh.vertices.size());
		for (auto& n : smoothNormals)
			n = Vector<float>{0.0f, 0.0f, 0.0f};

		for (size_t i = 0; i + 2 < submesh.indices.size(); i += 3) {
			size_t i0 = submesh.indices[i];
			size_t i1 = submesh.indices[i + 1];
			size_t i2 = submesh.indices[i + 2];

			if (i0 >= submesh.vertices.size() || i1 >= submesh.vertices.size() || i2 >= submesh.vertices.size())
				continue;
			
			Vector<float> p0 = submesh.vertices[i0].position;
			Vector<float> p1 = submesh.vertices[i1].position;
			Vector<float> p2 = submesh.vertices[i2].position;

			Vector<float> faceNormal = computeFaceNormal(p0, p1, p2);
			if (faceNormal.lengthSquared() < 1e-12f)
				continue;
	
			faceNormal = faceNormal.normalize();

			auto addToGroup = [&](size_t idx) {
				int xi = static_cast<int>(std::floor(submesh.vertices[idx].position[0] / eps));
				int yi = static_cast<int>(std::floor(submesh.vertices[idx].position[1] / eps));
				int zi = static_cast<int>(std::floor(submesh.vertices[idx].position[2] / eps));
				std::size_t h = hash_tuple(xi, yi, zi);
				for (size_t k : posGroups[h])
					smoothNormals[k] += faceNormal;
			};
				addToGroup(i0);
				addToGroup(i1);
				addToGroup(i2);
		}

		for (size_t i = 0; i < submesh.vertices.size(); ++i) {
			auto& n = submesh.vertices[i].normal;
			if (!usedVertices[i] || smoothNormals[i].lengthSquared() < 1e-12f)
				n = {0.0f, 1.0f, 0.0f};
			else
				n = smoothNormals[i].normalize();
		}
	}
}

std::string utils::resolveUri(MeshData& meshdata, int index) {
	int uriIndex = meshdata.textures[index].source;
	return meshdata.images[uriIndex].uri;
}

//FAUT REFACTO C HORRIBLE
void utils::prepareMats(MeshData& meshdata, TextureManager& texMng) {
	for (auto& mat : meshdata.materials) {

		//baseColor
		if (mat.pbrMetallicRoughness.baseColorTexture.index >= 0) {
			std::string uri = resolveUri(meshdata, mat.pbrMetallicRoughness.baseColorTexture.index);
			std::cout << "load d'une texture basique" << std::endl;
			mat.baseColorTextureGPU = &texMng.getOrLoad(uri);
			// std::cout << mat.baseColorTextureGPU << std::endl;
		}

		//metallic roughness
		// if (mat.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
		// 	std::string uri = resolveUri(meshdata, mat.pbrMetallicRoughness.metallicRoughnessTexture.index);
		// 	std::cout << "load une texture metallic" << std::endl;
		// 	mat.metallicRoughnessTextureGPU = &texMng.getOrLoad(uri);
		// }

		//normal
		if (mat.normalTexture.index >= 0) {
			std::string uri = resolveUri(meshdata, mat.normalTexture.index);
			std::cout << "load d'une texture de normale" << std::endl;
			mat.normalTextureGPU = &texMng.getOrLoad(uri);
		}

		//occlusion
		if (mat.occlusionTexture.index >= 0) {
			std::string uri = resolveUri(meshdata, mat.occlusionTexture.index);
			mat.occlusionTextureGPU = &texMng.getOrLoad(uri);
			// std::cout << mat.occlusionTextureGPU << std::endl;
		}

		// emissive
		if (mat.emissiveTexture.index >= 0) {
			std::string uri = resolveUri(meshdata, mat.emissiveTexture.index);
			mat.emissiveTextureGPU = &texMng.getOrLoad(uri);
		}

		//Ka
		if (!mat.map_Ka.empty())
			mat.map_KaGPU = &texMng.getOrLoad(mat.map_Ka);

		//Kd
		if (!mat.map_Kd.empty())
			mat.map_KdGPU = &texMng.getOrLoad(mat.map_Kd);

		//Ks
		if (!mat.map_Ks.empty())
			mat.map_KsGPU = &texMng.getOrLoad(mat.map_Ks);

		//Ns
		if (!mat.map_Ns.empty())
			mat.map_NsGPU = &texMng.getOrLoad(mat.map_Ns);

		//d
		if (!mat.map_d.empty())
			mat.map_dGPU = &texMng.getOrLoad(mat.map_d);

		//bump
		if (!mat.bump.empty())
			mat.bumpGPU = &texMng.getOrLoad(mat.bump);

	}
};
