#include <Skybox.hpp>

Skybox::Skybox() : shaders("srcs/skybox.vs", "srcs/skybox.fs") {
	this->skyboxVertices = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
	};

	TextureList.resize(6);
	TextureList[0].name = "resources/xneg.png";
	TextureList[1].name = "resources/xpos.png";
	TextureList[2].name = "resources/yneg.png";
	TextureList[3].name = "resources/ypos.png";
	TextureList[4].name = "resources/zneg.png";
	TextureList[5].name = "resources/zpos.png";

	GLenum faces[6] = {
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, //xneg
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, //xpos
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, //yneg
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, //ypos
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, //zneg
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z  //zpos
	};

	glGenVertexArrays(1, &this->skyboxVAO);
	glGenBuffers(1, &this->skyboxVBO);

	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, this->skyboxVertices.size() * sizeof(float), skyboxVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	glGenTextures(1, &this->cubeMaptexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubeMaptexture);

	for (size_t i = 0; i < TextureList.size(); ++i)
	{
		TextureList[i].tex.loadTexture(TextureList[i].name);
		glTexImage2D(faces[i], 0, GL_RGBA, TextureList[i].tex.getwidth(), TextureList[i].tex.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureList[i].tex.getData().data());
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	shaders.bind();
	shaders.setInt("skybox", 0);
};

void Skybox::draw(Matrix<float> view, Matrix<float>& projection) {

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	shaders.bind();
	// glUseProgram(this->shaderID);

	shaders.setMatrix4("view", view.datal());
	shaders.setMatrix4("projection", projection.datal());
	// glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_TRUE, view.datal());
	// glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_TRUE, projection.datal());

	glBindVertexArray(this->skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMaptexture);
	glDrawArrays(GL_TRIANGLES,0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
};



Skybox::~Skybox() {
	glDeleteVertexArrays(1, &this->skyboxVAO);
    glDeleteBuffers(1, &this->skyboxVBO);
    glDeleteTextures(1, &this->cubeMaptexture);
};

