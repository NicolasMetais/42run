#include <Renderer/Renderer.hpp>
#include <Renderer/VertexAttrib.hpp>

static const int DEBUG_MODE =
#ifdef DEBUG_WHITE
    1;
#else
    0;
#endif

Renderer::Renderer() : gltfShader("srcs/Renderer/gltf.vs", "srcs/Renderer/gltf.fs"), objShader("srcs/Renderer/obj.vs", "srcs/Renderer/obj.fs"), debug("srcs/Renderer/debug.vs", "srcs/Renderer/debug.fs") {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glEnable(GL_MULTISAMPLE); // MSAA — les samples sont demandes dans Window (SDL_GL_MULTISAMPLESAMPLES)
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
		if (subMesh.hasJoints) {
			for (int j : v.joints) flat.push_back((float)j);
		}
		if (subMesh.hasWeights) {
			for (float w : v.weights) flat.push_back(w);
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
	if (subMesh.hasColor) {
		for (size_t i = 0; i < subMesh.colorCount; ++i) {
			layout.attributes.push_back({attribIndex++, 3, GL_FLOAT, false, offset});
			offset += 4 * sizeof(float);
		}
	}
	// Pad locations to 7 so joints/weights are always at fixed locations 7 and 8
	attribIndex = 7;

	if (subMesh.hasJoints) {
		layout.attributes.push_back({attribIndex++, 4, GL_FLOAT, false, offset});
		offset += 4 * sizeof(float);
	} else attribIndex++;
	if (subMesh.hasWeights) {
		layout.attributes.push_back({attribIndex++, 4, GL_FLOAT, false, offset});
		offset += 4 * sizeof(float);
	} else attribIndex++;

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
	shader.setInt("debugMode", DEBUG_MODE);

	Vector<float> cam = camera.getCameraPos();
	shader.setVec3("viewPos", cam.x(), cam.y(), cam.z()); //pos de la camera
	// Vector<float> ligthDir{-0.5f, -1.0f, -0.3f};
	Vector<float> ligthDir{-1.0f, -0.3f, -1.0f};
	ligthDir = ligthDir.normalize();
	shader.setVec3("lightDir", ligthDir.x(), ligthDir.y(), ligthDir.z());
};

void Renderer::sendMaterialUniforms(Shader& shader, const Mat* mat, SubMesh& mesh, GLuint skyboxId, GLuint prefilterMapId) {
	int texSlot = 0; //incremente par bindTexture

	if (!mat) {
		shader.setVec3("Kd", 0.8f, 0.8f, 0.8f);
		shader.setVec3("Ka", 0.1f, 0.1f, 0.1f);
		shader.setVec3("Ks", 0.2f, 0.2f, 0.2f);
		shader.setFloat("Ni", 1.0f);
		shader.setInt("Ns", 32);
		shader.setInt("illum", 0);
		shader.setFloat("d", 1.0f);
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
		shader.setFloat("Ni", mat->Ni);
		shader.setInt("Ns", mat->Ns);
		shader.setInt("illum", mat->illum);
		shader.setFloat("d", mat->d);
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
		shader.setFloat("metallicFactor", metallic);
		shader.setFloat("roughnessFactor", roughness);

		shader.setVec3("emissiveFactor", mat->emissiveFactor[0], mat->emissiveFactor[1], mat->emissiveFactor[2]);

		shader.setFloat("normalScale", mat->normalTextureScale);
		shader.setFloat("occlusionStrength", mat->occlusionStrength);
		shader.setFloat("alphaCutOff", mat->alphaCutoff);

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
		shader.setFloat("lightIntensity", 1.0f);
		glActiveTexture(GL_TEXTURE20);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxId);
		shader.setInt("irradianceMap", 20);
		glActiveTexture(GL_TEXTURE21);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMapId);
		shader.setInt("prefilterMap", 21);

	}

};

void Renderer::draw(SubMesh& mesh, const Mat* mat) {
    // std::cout << "Drawing mesh. EBO: " << mesh.EBO << ", indices: " << mesh.indices.size() << std::endl;
    glBindVertexArray(mesh.VAO);
    if (mat && mat->doubleSided)
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
    glBindVertexArray(0);
};

void Renderer::rendering(Matrix<float>& mvp, MeshData& obj, Matrix<float> model, Camera& camera, GLuint skyboxId, GLuint prefilterMapId, const std::vector<Matrix<float>>* jointMats, RenderPass pass) {
	for (auto& mesh : obj.submeshes) {
		const Mat* mat = (mesh.materialIndex >= 0 && mesh.materialIndex < (int)obj.materials.size())
			? &obj.materials[mesh.materialIndex] : nullptr;

		bool isBlend = mat && mat->alphaMode == "BLEND";
		if ((pass == RenderPass::Transparent) != isBlend) continue;

		Shader& shader = (mat && mat->type == MaterialType::PBR) ? this->gltfShader : this->objShader;
		shader.bind();
		sendCommonUniforms(shader, mvp, model, camera);
		sendMaterialUniforms(shader, mat, mesh, skyboxId, prefilterMapId);

		if (jointMats && !jointMats->empty()) {
			shader.setInt("hasSkin", 1);
			std::vector<float> flat;
			flat.reserve(jointMats->size() * 16);
			for (const auto& m : *jointMats)
				for (float f : m.data) flat.push_back(f);
			GLint loc = glGetUniformLocation(shader.getId(), "jointMatrices");
			glUniformMatrix4fv(loc, (GLsizei)jointMats->size(), GL_TRUE, flat.data());
		} else {
			shader.setInt("hasSkin", 0);
		}

		draw(mesh, mat);
	}
};

void Renderer::beginOpaquePass() {
	glDisable(GL_BLEND);
};

void Renderer::beginTransparentPass() {
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
};

void Renderer::endTransparentPass() {
	// le blend reste ON : TextRenderer/UIRenderer et la skybox comptent dessus
	glDepthMask(GL_TRUE);
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
