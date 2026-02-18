#include <Renderer.hpp>
#include <VertexAttrib.hpp>


Renderer::Renderer() : gltfShader(), objShader() {
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    	throw std::runtime_error("Failed to initialize GLAD");
	printf("OpenGL version: %s\n", glGetString(GL_VERSION));
	printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	gltfShader = Shader("srcs/gltf.vs", "srcs/gltf.fs");
	objShader = Shader("srcs/obj.vs", "srcs/obj.fs");
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glCullFace(GL_BACK);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
};

std::vector<float> flattenVertexData(SubMesh& subMesh) {
	std::vector<float> flat;

	for (const auto& v : subMesh.vertices) {
		if (subMesh.hasPos)
		for (size_t i = 0; i < 3; ++i) flat.push_back(v.position[i]);

		if (subMesh.hasNormal)
			for (size_t i = 0; i < 3; ++i) flat.push_back(v.normal[i]);

		if (subMesh.hasTangent)
			for (int i = 0; i < 4; ++i) flat.push_back(v.tangent[i]);

		if (subMesh.hasTexCoord) {
			for (size_t i = 0; i < subMesh.texCoordCount; ++i) {
				flat.push_back(v.uv[i][0]);
				flat.push_back(v.uv[i][1]);
			}
		}
		if (subMesh.hasColor) {
			for (auto f : v.color)
				flat.push_back(f);
		}
	}
	return flat;
}

void Renderer::InitMesh(SubMesh& subMesh) {
	if (subMesh.vertices.empty()) 
		return;
	glGenVertexArrays(1, &subMesh.VAO);
	glBindVertexArray(subMesh.VAO);
	
	glGenBuffers(1, &subMesh.VBO);
	glBindBuffer(GL_ARRAY_BUFFER , subMesh.VBO);

	std::vector<float> flatData = flattenVertexData(subMesh);
	glBufferData(GL_ARRAY_BUFFER, flatData.size() * sizeof(float), flatData.data(), GL_STATIC_DRAW);
	if (!subMesh.indices.empty()) {
		glGenBuffers(1, &subMesh.EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMesh.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, subMesh.indices.size() * sizeof(uint32_t), subMesh.indices.data(), GL_STATIC_DRAW);
	}

	VertexLayout layout;
	size_t offset = 0;
	GLuint attribIndex = 0;
	
	if (subMesh.hasPos) {
		layout.attributes.push_back({attribIndex++, 3, GL_FLOAT, false, offset});
		offset += 3 * sizeof(float);
	}
	if (subMesh.hasNormal) {
		layout.attributes.push_back({attribIndex++, 3, GL_FLOAT, false, offset});
		offset += 3 * sizeof(float);
	}
	if (subMesh.hasTangent) {
		layout.attributes.push_back({attribIndex++, 4, GL_FLOAT, false, offset});
		offset += 4 * sizeof(float);
	}
	if (subMesh.hasTexCoord) {
		for (size_t i = 0; i < subMesh.texCoordCount; ++i) {
			layout.attributes.push_back({attribIndex++, 2, GL_FLOAT, false, offset});
			offset += 2 * sizeof(float);
		}
	}
	if (subMesh.hasColor) {
		for (size_t i = 0; i < subMesh.colorCount; ++i) {
			layout.attributes.push_back({attribIndex++, 3, GL_FLOAT, false, offset});
			offset += 3 * sizeof(float);
		}
	}

	layout.stride = offset;

	for (const auto& attr : layout.attributes) {
		glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, layout.stride, (void*)attr.offset);
		glEnableVertexAttribArray(attr.index);
	}
	glBindVertexArray(0);
};

void Renderer::bindTexture(int& texSlot, GLuint loc, GLuint flagLoc, const Texture* texture)  {

	if (texture) {
		glActiveTexture(GL_TEXTURE0 + texSlot);
		glUniform1i(loc, texSlot);
		glUniform1i(flagLoc, 1);
		texture->bind();
		texSlot++;
	} else
		glUniform1i(flagLoc, 0);
}

void Renderer::sendCommonUniforms(Shader& shader, Matrix<float>& mvp, Matrix<float>& model, Camera& camera) {
	shader.setMatrix4("MVP", mvp.datal());
	shader.setMatrix4("model", model.datal());

	Vector<float> cam = camera.getCameraPos();
	shader.setVec3("viewPos", cam.x(), cam.y(), cam.z()); //pos de la camera
};

void Renderer::sendMaterialUniforms(Shader& shader, const Mat* mat, SubMesh& mesh) {
	int texSlot = 0; //incremente par bindTexture

	if (!mat) {
		shader.setVec3("Kd", 0.8f, 0.8f, 0.8f);
		shader.setVec3("Ka", 0.1f, 0.1f, 0.1f);
		shader.setVec3("Ks", 0.2f, 0.2f, 0.2f);
		shader.setfloat("Ni", 1.0f);
		shader.setInt("Ns", 32);
		shader.setInt("illum", 0);
		shader.setfloat("d", 1.0f);
		shader.setInt("hasMtl", 0);

		shader.setInt("isMap_Ka", 0); //attention faut recheck si j'envois les bon trucs c'pas clair
		shader.setInt("isMap_Kd", 0);
		shader.setInt("isMap_Ks", 0);
		shader.setInt("isMap_Ns", 0);
		shader.setInt("isMap_d", 0);
		shader.setInt("isBump", 0);
		shader.setVec3("lightColor", 1.0f, 0.0f, 1.0f);
		shader.setVec3("lightDir", -0.5f, -1.0f, -0.3f);
		return ;
	} else if (mat->type == MaterialType::PHONG) {
		shader.setVec3("Kd", mat->Kd.x(), mat->Kd.y(), mat->Kd.z());
		shader.setVec3("Ka", mat->Ka.x(), mat->Ka.y(), mat->Ka.z());
		shader.setVec3("Ks", mat->Ks.x(), mat->Ks.y(), mat->Ks.z());
		shader.setfloat("Ni", mat->Ni);
		shader.setInt("Ns", mat->Ns);
		shader.setInt("illum", mat->illum);
		shader.setfloat("d", mat->d);
		shader.setInt("hasMtl", 1);
		shader.setVec3("lightColor", 1.0f, 0.0f, 1.0f);
		shader.setVec3("lightDir", -0.5f, -1.0f, -0.3f);


		bindTexture(texSlot, shader.getUniformLocation("map_Ka"), shader.getUniformLocation("isMap_Ka"), mat->map_KaGPU);
		bindTexture(texSlot, shader.getUniformLocation("map_Kd"), shader.getUniformLocation("isMap_Kd"), mat->map_KdGPU);
		bindTexture(texSlot, shader.getUniformLocation("map_Ks"), shader.getUniformLocation("isMap_Ks"), mat->map_KsGPU);
		bindTexture(texSlot, shader.getUniformLocation("map_Ns"), shader.getUniformLocation("isMap_Ns"), mat->map_NsGPU);
		bindTexture(texSlot, shader.getUniformLocation("map_d"), shader.getUniformLocation("isMap_d"), mat->map_dGPU);
		bindTexture(texSlot, shader.getUniformLocation("bump"), shader.getUniformLocation("isBump"), mat->bumpGPU);
	} else {
		auto& pbr = mat->pbrMetallicRoughness;

		float metallic = (pbr.metallicFactor < 0.0f) ? 1.0f : pbr.metallicFactor;
		float roughness = (pbr.roughnessFactor < 0.0f) ? 1.0f : pbr.roughnessFactor;

		shader.setVec4("BaseColorFactor", pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2], pbr.baseColorFactor[3]);
		shader.setfloat("MetallicFactor", metallic);
		shader.setfloat("RoughnessFactor", roughness);

		shader.setVec3("EmissiveFactor", mat->emissiveFactor[0], mat->emissiveFactor[1], mat->emissiveFactor[2]);

		shader.setfloat("NormalScale", mat->normalTextureScale);
		shader.setfloat("OcclusionStrength", mat->occlusionStrength);
		shader.setfloat("AlphaCutOff", mat->alphaCutoff);

		//Alphamode
		int alphaMode = 0;
		if (mat->alphaMode == "MASK") alphaMode = 1;
		if (mat->alphaMode == "BLEND") alphaMode = 2;
		shader.setInt("AlphaMode", alphaMode);
		bindTexture(texSlot, shader.getUniformLocation("BaseColorTex"), shader.getUniformLocation("hasBaseColorTexture"), mat->baseColorTextureGPU);
		bindTexture(texSlot, shader.getUniformLocation("MetallicRougnessTex"), shader.getUniformLocation("hasMetallicRougnessTexture"), mat->metallicRoughnessTextureGPU);
		bindTexture(texSlot, shader.getUniformLocation("NormalMap"), shader.getUniformLocation("hasNormalMap"), mat->normalTextureGPU);
		bindTexture(texSlot, shader.getUniformLocation("OcclusionTex"), shader.getUniformLocation("hasOcclusionTexture"), mat->occlusionTextureGPU);
		bindTexture(texSlot, shader.getUniformLocation("EmissiveTex"), shader.getUniformLocation("hasEmissiveTexture"), mat->emissiveTextureGPU);

		shader.setInt("hasTangent", mesh.hasTangent);
		shader.setInt("hasVertexColor", mesh.hasColor);

	}

};

void Renderer::draw(SubMesh& mesh) {
	glBindVertexArray(mesh.VAO);
	if (mesh.material && mesh.material->doubleSided)
		glDisable(GL_CULL_FACE);
	else
		glEnable(GL_CULL_FACE);
	if (!mesh.EBO)
		glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
	else {
		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
};



void Renderer::rendering(Matrix<float>& mvp, MeshData& obj, Matrix<float> model, Camera& camera) {
	for (auto& mesh : obj.submeshes) {
		const Mat* mat = mesh.material;

		Shader& shader = (mat && mat->type == MaterialType::PBR) ? this->gltfShader : this->objShader;
		shader.bind();

		sendCommonUniforms(shader, mvp, model, camera);
		sendMaterialUniforms(shader, mat, mesh);
		draw(mesh);
	}
};

void Renderer::cleanup(MeshData& obj) {
	for (auto& mesh : obj.submeshes) {
		if (mesh.VAO)
			glDeleteVertexArrays(1, &mesh.VAO);
		if (mesh.VBO)
			glDeleteBuffers(1, &mesh.VBO);
		if (mesh.EBO)
			glDeleteBuffers(1, &mesh.EBO);
			mesh.VAO = 0;
			mesh.VBO = 0;
			mesh.EBO = 0;
		}

	if (this->gltfShader.getId())
		glDeleteProgram(this->gltfShader.getId());
	if (this->objShader.getId())
		glDeleteProgram(this->objShader.getId());
};
