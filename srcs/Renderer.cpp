#include <Renderer.hpp>

Renderer::Renderer() : gltfShader("srcs/gltf.fs", "srcs/gltf.vs"), objShader("srcs/obj.fs", "srcs/obj.vs") {
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    	throw std::runtime_error("Failed to initialize GLAD");
};

std::vector<float> flattenVertexData(const std::vector<Vertex>& vertices) {
	std::vector<float> flat;
	flat.reserve(vertices.size() * (3+3+2+3));
	for (const auto& v : vertices) {
		for (size_t i = 0; i < 3; ++i) flat.push_back(v.position[i]);

		for (size_t i = 0; i < 3; ++i) flat.push_back(v.normal[i]);

		// if (v.tangent.size() == 4) {
		// 	for (int i = 0; i < 4; ++i) flat.push_back(v.tangent[i]);
		// } else {
		// 	// std::cout << "PAS DE TANGENT WESH" << std::endl;
		// 	flat.push_back(1.0f); flat.push_back(0.0f); flat.push_back(0.0f); flat.push_back(0.0f);
		// }

		if (!v.uv.empty()) {
			flat.push_back(v.uv[0][0]);
			flat.push_back(v.uv[0][1]);
		} else {
			flat.push_back(0.0f);
			flat.push_back(0.0f);
		}
		if ((!v.color.empty())) {
			// std::cout << "color exist" << std::endl;
			flat.insert(flat.end(), v.color.begin(), v.color.end());
		} else {
			// std::cout << "color empty" << std::endl;
			flat.push_back(1.0f);
			flat.push_back(1.0f);
			flat.push_back(1.0f);
		}
	}
	return flat;
}

void Renderer::InitObj(MeshData& obj) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (auto& mesh : obj.submeshes) {
		//mesh.vertexCount = mesh.vertices.size() / 11;
		glGenVertexArrays(1, &mesh.VAO);
		glBindVertexArray(mesh.VAO);
		
		glGenBuffers(1, &mesh.VBO);
		glBindBuffer(GL_ARRAY_BUFFER , mesh.VBO);

		std::vector<float> flatData = flattenVertexData(mesh.vertices);
		glBufferData(GL_ARRAY_BUFFER, flatData.size() * sizeof(float), flatData.data(), GL_STATIC_DRAW);

		if (!mesh.indices.empty()) {
			glGenBuffers(1, &mesh.EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);
		}

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glBindVertexArray(0);
	}
};

 void Renderer::toggleTexture() {
	if (isTransitionning())
		return ;
	useTexture = !useTexture; 
	transitionTarget = useTexture ? 1.0f : 0.0f; 
};


void Renderer::bindTexture(int& texSlot, GLuint loc, GLuint flagLoc, const Texture* texture)  {

	if (texture) {
		glActiveTexture(GL_TEXTURE0 + texSlot);
		glUniform1i(loc, texSlot);
		glUniform1i(flagLoc, texSlot);
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

void Renderer::sendMaterialUniforms(Shader& shader, const Mat* mat) {
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
		return ;
	} else if (mat->type == MaterialType::PHONG) {
		shader.setVec3("Kd", mat->Kd.x(), mat->Kd.y(), mat->Kd.z());
		shader.setVec3("Ka", mat->Ka.x(), mat->Ka.y(), mat->Ka.z());
		shader.setVec3("Ks", mat->Ks.x(), mat->Ks.y(), mat->Ks.x());
		shader.setfloat("Ni", mat->Ni);
		shader.setInt("Ns", mat->Ns);
		shader.setInt("illum", mat->illum);
		shader.setfloat("d", mat->d);
		shader.setInt("hasMtl", 1);

		int texSlot = 1; //incremente par bindTexture
		bindTexture(texSlot, shader.getUniformLocation("map_Ka"), shader.getUniformLocation("isMap_Ka"), mat->map_KaGPU);
		bindTexture(texSlot, shader.getUniformLocation("map_Kd"), shader.getUniformLocation("isMap_Kd"), mat->map_KdGPU);
		bindTexture(texSlot, shader.getUniformLocation("map_Ks"), shader.getUniformLocation("isMap_Ks"), mat->map_KsGPU);
		bindTexture(texSlot, shader.getUniformLocation("map_Ns"), shader.getUniformLocation("isMap_Ns"), mat->map_NsGPU);
		bindTexture(texSlot, shader.getUniformLocation("map_d"), shader.getUniformLocation("isMap_d"), mat->map_dGPU);
		bindTexture(texSlot, shader.getUniformLocation("bump"), shader.getUniformLocation("isBump"), mat->bumpGPU);
	} else {
		//gltf uniforms
	}

};



void Renderer::rendering(Matrix<float>& mvp, MeshData& obj, Matrix<float> model, Camera& camera, float deltaTime) {
	for (auto& mesh : obj.submeshes) {
		const Mat* mat = mesh.material;

		Shader& shader = (mat && mat->type == MaterialType::PBR) ? this->gltfShader : this->objShader;
		shader.bind();

		sendCommonUniforms(shader, mvp, model, camera);
		sendMaterialUniforms(shader, mat);
		draw(mesh);
	}

	glUniform3f(loc.lightColor, 1.0f, 0.0f, 1.0f); //couleur de la lumiere;
	glUniform3f(loc.lightDir, -0.5f, -1.0f, -0.3f); //direction de la lumiere

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(loc.scopTexture, 0);
	ScopTexture.bind();
	glUniform1i(loc.useTexture, (useTexture || isTransitionning()) ? 1 : 0);

	for (auto& mesh : obj.submeshes) {
		// if (mesh.material)
		// {
			// Vector<float> Kd = mesh.mat->getKd();
			// Vector<float> Ka = mesh.mat->getKa();
			// Vector<float> Ks = mesh.mat->getKs();
			// glUniform1i(loc.hasMtl, 1);
			// glUniform3f(loc.Kd, Kd.x(), Kd.y(), Kd.z());
			// glUniform3f(loc.Ka, Ka.x(), Ka.y(), Ka.z());
			// glUniform3f(loc.Ks, Ks.x(), Ks.y(), Ks.z());
			// glUniform1f(loc.Ns, mesh.mat->getNs());
			// glUniform1f(loc.Ni, mesh.mat->getNi());
			// glUniform1f(loc.d, mesh.mat->getd());
			// glUniform1i(loc.illum, mesh.mat->getIllum());
			// bindTexture(texSlot, loc.map_Ka, loc.isMap_Ka, mesh.mat->getMapKa());
			// bindTexture(texSlot, loc.map_Kd, loc.isMap_Kd, mesh.mat->getMapKd());
			// bindTexture(texSlot, loc.map_Ks, loc.isMap_Ks, mesh.mat->getMapKs());
			// bindTexture(texSlot, loc.map_Ns, loc.isMap_Ns, mesh.mat->getMapNs());
			// bindTexture(texSlot, loc.map_d, loc.isMap_d, mesh.mat->getMapd());
			// bindTexture(texSlot, loc.bump, loc.isBump, mesh.mat->getMapBump());
			// std::cout << "material detected" << std::endl;
		// }
		// else
		// {
			glUniform3f(loc.Kd, 0.8, 0.8, 0.8);
			glUniform3f(loc.Ka, 0.1, 0.1, 0.1);
			glUniform3f(loc.Ks, 0.2, 0.2, 0.2);
			glUniform1f(loc.Ni, 1.0);
			glUniform1f(loc.Ns, 32);
			glUniform1i(loc.illum, 0);
			glUniform1f(loc.d,1.0);
			glUniform1i(loc.hasMtl, 0);
			glUniform1i(loc.isMap_Ka, 0); //wrong variables ?
			glUniform1i(loc.isMap_Kd, 0);
			glUniform1i(loc.isMap_Ks, 0);
			glUniform1i(loc.isMap_Ns, 0);
			glUniform1i(loc.isMap_d, 0);
			glUniform1i(loc.isBump, 0);
		// }
		glBindVertexArray(mesh.VAO);
		GLenum err = glGetError();

		
		if (!mesh.EBO)
			glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
		else {
			// std::cout << mesh.indices.size() << std::endl;
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
		}
		err = glGetError();
		glBindVertexArray(0);
	}
};

void Renderer::cleanup(MeshData& obj) {
	for (auto& mesh : obj.submeshes) {
		if (mesh.VAO)
			glDeleteVertexArrays(1, &mesh.VAO);
		if (mesh.VBO)
			glDeleteBuffers(1, &mesh.VBO);
		}
	if (this->shaderProgram) //A CHANGER
		glDeleteProgram(this->shaderProgram);
};
