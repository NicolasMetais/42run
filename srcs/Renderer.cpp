#include <Renderer.hpp>
#include "shader.cpp"

Renderer::Renderer() {
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    	throw std::runtime_error("Failed to initialize GLAD");
	std::string vertexShader("srcs/mesh.vs");
	std::string fragmentShader("srcs/mesh.fs");

	shaderProgram = createShaderProgram(vertexShader, fragmentShader);

	loc.MVP = glGetUniformLocation(this->shaderProgram, "MVP");
    loc.model = glGetUniformLocation(this->shaderProgram, "model");
	loc.Kd = glGetUniformLocation(this->shaderProgram, "Kd");
	loc.map_Kd = glGetUniformLocation(this->shaderProgram, "map_Kd");
	loc.isMap_Kd = glGetUniformLocation(this->shaderProgram, "isMap_Kd");
  	loc.Ka = glGetUniformLocation(this->shaderProgram, "Ka");
	loc.map_Ka = glGetUniformLocation(this->shaderProgram, "map_Ka");
	loc.isMap_Ka = glGetUniformLocation(this->shaderProgram, "isMap_Ka");
    loc.Ks = glGetUniformLocation(this->shaderProgram, "Ks");
	loc.map_Ks = glGetUniformLocation(this->shaderProgram, "map_Ks");
	loc.isMap_Ks = glGetUniformLocation(this->shaderProgram, "isMap_Ks");
    loc.Ns = glGetUniformLocation(this->shaderProgram, "Ns");
	loc.map_Ns = glGetUniformLocation(this->shaderProgram, "map_Ns");
	loc.isMap_Ns = glGetUniformLocation(this->shaderProgram, "isMap_Ns");
    loc.Ni = glGetUniformLocation(this->shaderProgram, "Ni");
    loc.d = glGetUniformLocation(this->shaderProgram, "d");
	loc.map_d = glGetUniformLocation(this->shaderProgram, "map_d");
	loc.isMap_d = glGetUniformLocation(this->shaderProgram, "isMap_d");
	loc.bump = glGetUniformLocation(this->shaderProgram, "bump");
	loc.isBump = glGetUniformLocation(this->shaderProgram, "isBump");
    loc.illum = glGetUniformLocation(this->shaderProgram, "illum");
    loc.hasMtl = glGetUniformLocation(this->shaderProgram, "hasMtl");
    loc.lightDir = glGetUniformLocation(this->shaderProgram, "lightDir");
    loc.viewPos = glGetUniformLocation(this->shaderProgram, "viewPos");
    loc.lightColor = glGetUniformLocation(this->shaderProgram, "lightColor");
	loc.transition = glGetUniformLocation(this->shaderProgram, "transition");
	loc.scopTexture = glGetUniformLocation(this->shaderProgram, "scopTexture");
	loc.useTexture = glGetUniformLocation(this->shaderProgram, "useTexture");
	this->ScopTexture.loadTexture("resources/test.png");
	this->ScopTexture.openGl2DTextureGen();
};


void Renderer::InitObj(Mesh& obj) {
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (auto& mesh : obj.getMeshes()) {
		mesh.vertexCount = mesh.vertices.size() / 11;
		glGenVertexArrays(1, &mesh.VAO);
		glGenBuffers(1, &mesh.VBO);
	
		glBindVertexArray(mesh.VAO);
		glBindBuffer(GL_ARRAY_BUFFER , mesh.VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
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


void Renderer::bindTexture(int& texSlot, GLuint loc, GLuint flagLoc, const std::optional<Texture>& texture)  {

	if (texture.has_value()) {
		glActiveTexture(GL_TEXTURE0 + texSlot);
		glUniform1i(loc, texSlot);
		glUniform1i(flagLoc, texSlot);
		texture->bind();
		texSlot++;
	} else
		glUniform1i(flagLoc, 0);
}

void Renderer::renderObj(Matrix<float>& mvp, Mesh& obj, Matrix<float> model, Camera& camera, float deltaTime) {
	glUseProgram(this->shaderProgram);

	glUniformMatrix4fv(loc.MVP, 1, GL_TRUE, mvp.datal());
	glUniformMatrix4fv(loc.model, 1, GL_TRUE, model.datal());

	Vector<float> cam = camera.getCameraPos();
	glUniform3f(loc.viewPos, cam.x(), cam.y(), cam.z()); //pos de la camera
	glUniform3f(loc.lightColor, 1.0f, 0.0f, 1.0f); //couleur de la lumiere;
	glUniform3f(loc.lightDir, -0.5f, -1.0f, -0.3f); //direction de la lumiere

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(loc.scopTexture, 0);
	ScopTexture.bind();
	glUniform1i(loc.useTexture, (useTexture || isTransitionning()) ? 1 : 0);

	int texSlot = 1;

	if (transition < transitionTarget) {
		transition += deltaTime * transitionSpeed;
		if (transition > transitionTarget)
			transition = transitionTarget;
	} else if (transition > transitionTarget) {
		transition -= deltaTime * transitionSpeed;
		if (transition < transitionTarget)
			transition = transitionTarget;
	}
	glUniform1f(loc.transition, transition);
	for (auto& mesh : obj.getMeshes()) {
		if (mesh.mat)
		{
			Vector<float> Kd = mesh.mat->getKd();
			Vector<float> Ka = mesh.mat->getKa();
			Vector<float> Ks = mesh.mat->getKs();
			glUniform1i(loc.hasMtl, 1);
			glUniform3f(loc.Kd, Kd.x(), Kd.y(), Kd.z());
			glUniform3f(loc.Ka, Ka.x(), Ka.y(), Ka.z());
			glUniform3f(loc.Ks, Ks.x(), Ks.y(), Ks.z());
			glUniform1f(loc.Ns, mesh.mat->getNs());
			glUniform1f(loc.Ni, mesh.mat->getNi());
			glUniform1f(loc.d, mesh.mat->getd());
			glUniform1i(loc.illum, mesh.mat->getIllum());
			bindTexture(texSlot, loc.map_Ka, loc.isMap_Ka, mesh.mat->getMapKa());
			bindTexture(texSlot, loc.map_Kd, loc.isMap_Kd, mesh.mat->getMapKd());
			bindTexture(texSlot, loc.map_Ks, loc.isMap_Ks, mesh.mat->getMapKs());
			bindTexture(texSlot, loc.map_Ns, loc.isMap_Ns, mesh.mat->getMapNs());
			bindTexture(texSlot, loc.map_d, loc.isMap_d, mesh.mat->getMapd());
			bindTexture(texSlot, loc.bump, loc.isBump, mesh.mat->getMapBump());
		}
		else
		{
			glUniform3f(loc.Kd, 0.8, 0.8, 0.8);
			glUniform3f(loc.Ka, 0.1, 0.1, 0.1);
			glUniform3f(loc.Ks, 0.2, 0.2, 0.2);
			glUniform1f(loc.Ni, 1.0);
			glUniform1f(loc.Ns, 32);
			glUniform1i(loc.illum, 2);
			glUniform1f(loc.d,1.0);
			glUniform1i(loc.hasMtl, 0);
			glUniform1i(loc.map_Ka, 0);
			glUniform1i(loc.map_Kd, 0);
			glUniform1i(loc.map_Ks, 0);
			glUniform1i(loc.map_Ns, 0);
			glUniform1i(loc.map_d, 0);
			glUniform1i(loc.bump, 0);
		}
		glBindVertexArray(mesh.VAO);
		GLenum err = glGetError();
		glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
		err = glGetError();
		glBindVertexArray(0);
	}
};

void Renderer::cleanup(Mesh& obj) {
	for (auto& mesh : obj.getMeshes()) {
		if (mesh.VAO)
			glDeleteVertexArrays(1, &mesh.VAO);
		if (mesh.VBO)
			glDeleteBuffers(1, &mesh.VBO);
		}
	if (this->shaderProgram)
		glDeleteProgram(this->shaderProgram);
};
