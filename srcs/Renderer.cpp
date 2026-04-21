#include <Renderer.hpp>
#include <VertexAttrib.hpp>

Renderer::Renderer() : gltfShader("srcs/gltf.vs", "srcs/gltf.fs"), objShader("srcs/obj.vs", "srcs/obj.fs"), debug("srcs/debug.vs", "srcs/debug.fs") {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	// glEnable(GL_MULTISAMPLE); //Antialiasing mais faut coder des trucs
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
	 glBindBuffer(GL_ARRAY_BUFFER, subMesh.VBO);

	VertexLayout layout;
	size_t offset = 0;
	GLuint attribIndex = 0;

	if (subMesh.hasPos) {
		// std::cout << "Pos" << std::endl;
		layout.attributes.push_back({attribIndex++, 3, GL_FLOAT, false, offset});
		offset += 3 * sizeof(float);
	}
	if (subMesh.hasNormal) {
		// std::cout << "Normal" << std::endl;
		layout.attributes.push_back({attribIndex++, 3, GL_FLOAT, false, offset});
		offset += 3 * sizeof(float);
	} else {
		attribIndex++;  // Skip normal slot even if not present
	}

	if (subMesh.hasTangent) {
		// std::cout << "Tangent" << std::endl;
		layout.attributes.push_back({attribIndex++, 4, GL_FLOAT, false, offset});
		offset += 4 * sizeof(float);
	} else {
		attribIndex++;  // Skip tangent slot even if not present
	}
	if (subMesh.hasTexCoord) {
		for (size_t i = 0; i < subMesh.texCoordCount; ++i) {
			// std::cout << "TexCoord" << std::endl;
			layout.attributes.push_back({attribIndex++, 2, GL_FLOAT, false, offset});
			offset += 2 * sizeof(float);
		}
	}
	if (subMesh.hasColor) { //la couleur peux etre un vec4 donc offset de 4 a GERER
		for (size_t i = 0; i < subMesh.colorCount; ++i) {
			// std::cout << "Color" << std::endl;
			layout.attributes.push_back({attribIndex++, 3, GL_FLOAT, false, offset});
			offset += 3 * sizeof(float);
		}
	}

	layout.stride = offset;
	for (const auto& attr : layout.attributes) {
		glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, layout.stride, (void*)attr.offset);
		glEnableVertexAttribArray(attr.index);
	}
};

void Renderer::bindTexture(int& texSlot, GLuint loc, GLuint flagLoc, const Texture* texture)  {
	if (texture) {
		glActiveTexture(GL_TEXTURE0 + texSlot);
		glUniform1i(loc, texSlot);
		glUniform1i(flagLoc, 1);
		texture->bind();
		texSlot++;
	} else {
		glUniform1i(loc, 0);
		glUniform1i(flagLoc, 0);
	}
}

void Renderer::sendCommonUniforms(Shader& shader, Matrix<float>& mvp, Matrix<float>& model, Camera& camera) {
	// std::cout << "Sending common uniforms to shader" << std::endl;
	shader.setMatrix4_true("MVP", mvp.datal());
	shader.setMatrix4_true("model", model.datal());

	Vector<float> cam = camera.getCameraPos();
	shader.setVec3("viewPos", cam.x(), cam.y(), cam.z()); //pos de la camera
	// Vector<float> ligthDir{-0.5f, -1.0f, -0.3f};
	Vector<float> ligthDir{-1.0f, -0.3f, 0.0f};
	ligthDir = ligthDir.normalize();
	shader.setVec3("lightDir", ligthDir.x(), ligthDir.y(), ligthDir.z());
};

void Renderer::sendMaterialUniforms(Shader& shader, const Mat* mat, SubMesh& mesh, GLuint skyboxId) {
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


		bindTexture(texSlot, shader.getUniformLocation("map_Ka"), shader.getUniformLocation("isMap_Ka"), mat->map_KaGPU);
		bindTexture(texSlot, shader.getUniformLocation("map_Kd"), shader.getUniformLocation("isMap_Kd"), mat->map_KdGPU);
		bindTexture(texSlot, shader.getUniformLocation("map_Ks"), shader.getUniformLocation("isMap_Ks"), mat->map_KsGPU);
		bindTexture(texSlot, shader.getUniformLocation("map_Ns"), shader.getUniformLocation("isMap_Ns"), mat->map_NsGPU);
		bindTexture(texSlot, shader.getUniformLocation("map_d"), shader.getUniformLocation("isMap_d"), mat->map_dGPU);
		bindTexture(texSlot, shader.getUniformLocation("bump"), shader.getUniformLocation("isBump"), mat->bumpGPU);
	} else {
		auto& pbr = mat->MetallicRoughness;

		float metallic = (pbr.metallicFactor < 0.0f) ? 1.0f : pbr.metallicFactor;
		float roughness = (pbr.roughnessFactor < 0.0f) ? 1.0f : pbr.roughnessFactor;

		shader.setVec4("baseColorFactor", pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2], pbr.baseColorFactor[3]);
		shader.setfloat("metallicFactor", metallic);
		shader.setfloat("roughnessFactor", roughness);

		shader.setVec3("emissiveFactor", mat->emissiveFactor[0], mat->emissiveFactor[1], mat->emissiveFactor[2]);

		shader.setfloat("normalScale", mat->normalTextureScale);
		shader.setfloat("occlusionStrength", mat->occlusionStrength);
		shader.setfloat("alphaCutOff", mat->alphaCutoff);

		//Alphamode
		int alphaMode = 0;
		if (mat->alphaMode == "MASK") alphaMode = 1;
		if (mat->alphaMode == "BLEND") alphaMode = 2;
		shader.setInt("alphaMode", alphaMode);

		bindTexture(texSlot, shader.getUniformLocation("baseColorTex"), shader.getUniformLocation("hasBaseColorTexture"), mat->baseColorTextureGPU);
		bindTexture(texSlot, shader.getUniformLocation("metallicRougnessTex"), shader.getUniformLocation("hasMetallicRoughnessTexture"), mat->metallicRoughnessTextureGPU);
		bindTexture(texSlot, shader.getUniformLocation("normalMap"), shader.getUniformLocation("hasNormalMap"), mat->normalTextureGPU);
		bindTexture(texSlot, shader.getUniformLocation("occlusionTex"), shader.getUniformLocation("hasOcclusionTexture"), mat->occlusionTextureGPU);
		bindTexture(texSlot, shader.getUniformLocation("emissiveTex"), shader.getUniformLocation("hasEmissiveTexture"), mat->emissiveTextureGPU);
		// shader.setInt("hasBaseColorTexture", 1); //forcing pour tester
		shader.setInt("hasTangent", mesh.hasTangent);
		shader.setInt("hasVertexColor", mesh.hasColor);
		shader.setfloat("lightIntensity", 1.0f);
		glActiveTexture(GL_TEXTURE20);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxId);
		shader.setInt("irradianceMap", 20);
	}

};

void Renderer::draw(SubMesh& mesh) {
    // std::cout << "Drawing mesh. EBO: " << mesh.EBO << ", indices: " << mesh.indices.size() << std::endl;
    glBindVertexArray(mesh.VAO);
	GLenum err = glGetError();
    // if (err != GL_NO_ERROR) std::cerr << "GL Error after bind: " << err << std::endl;
    if (mesh.material && mesh.material->doubleSided)
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);
	// std::cout << "VAO=" << mesh.VAO 
    //       << " indexType=" << mesh.indexType 
    //       << " count=" << mesh.indices.size() << std::endl;
	// GLint currentVAO;
	// glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
	// std::cout << "GL_VERTEX_ARRAY_BINDING=" << currentVAO << std::endl;
    if (!mesh.EBO) {
        // std::cout << "Using glDrawArrays" << std::endl;
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
    } else {
        // std::cout << "Using glDrawElements with " << mesh.indices.size() << " indices" << std::endl;
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    }
	// glDrawArrays(GL_TRIANGLES, 0, 3);
	err = glGetError();
    if (err != GL_NO_ERROR) std::cerr << "GL Error after draw: " << err << std::endl;
    glBindVertexArray(0);
};

void Renderer::rendering(Matrix<float>& mvp, MeshData& obj, Matrix<float> model, Camera& camera, GLuint skyboxId) {
	for (auto& mesh : obj.submeshes) {
		const Mat* mat = mesh.material;

		Shader& shader = (mat && mat->type == MaterialType::PBR) ? this->gltfShader : this->objShader;
		// Shader& shader = this->debug; //shader simple de debug
		shader.bind();
		sendCommonUniforms(shader, mvp, model, camera);
		sendMaterialUniforms(shader, mat, mesh, skyboxId);
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
