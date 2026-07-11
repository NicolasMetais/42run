#include <Skybox/Skybox.hpp>

static const int DEBUG_MODE =
#ifdef DEBUG_WHITE
    1;
#else
    0;
#endif

Skybox::Skybox() : shaders("srcs/Skybox/skybox.vs", "srcs/Skybox/skybox.fs"), hdr("resources/skybox.hdr"){
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
	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLuint hdrTex;
	glGenTextures(1, &hdrTex);
	glBindTexture(GL_TEXTURE_2D, hdrTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, hdr.getWidth(), hdr.getHeight(), 0, GL_RGB, GL_FLOAT, hdr.getData().data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Matrix<float> captureProjection = utils::perspective(90.0f * (3.1415926f / 180.0f), 1.0f, 0.1f, 10.0f);

	Shader equirec("srcs/Skybox/irradiance.vs", "srcs/Skybox/equirec.fs");
	equirec.bind();

	equirec.setInt("equirectMap", 0);
	equirec.setMatrix4_true("projection", captureProjection.datal());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTex);

	GLuint FBO, RBO;
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	Vector<float> origin = Vector<float>{0.0f, 0.0f, 0.0f};
	Matrix<float> captureViews[6] = {
		utils::view(origin, Vector<float>{1, 0, 0}, Vector<float>{0, -1, 0}),
		utils::view(origin, Vector<float>{-1, 0, 0}, Vector<float>{0, -1, 0}),
		utils::view(origin, Vector<float>{0, 1, 0}, Vector<float>{0, 0, 1}),
		utils::view(origin, Vector<float>{0, -1, 0}, Vector<float>{0, 0, -1}),
		utils::view(origin, Vector<float>{0, 0, 1}, Vector<float>{0, -1, 0}),
		utils::view(origin, Vector<float>{0, 0, -1}, Vector<float>{0, -1, 0}),
	};

	glViewport(0,0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	for(size_t i = 0; i < 6; ++i) {
		equirec.setMatrix4_true("view", captureViews[i].datal());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->cubeMaptexture, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(this->skyboxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteTextures(1, &hdrTex);
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);
	shaders.bind();
	shaders.setInt("skybox", 0);
};

void Skybox::generateIrradianceMap() {
	glGenTextures(1, &this->irradianceMapId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->irradianceMapId);

	for (size_t i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLuint fbo, rbo;
	glGenFramebuffers(1, &fbo);
	glGenRenderbuffers(1, &rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	Matrix<float> captureProjection = utils::perspective(90.0f * (3.1415926f / 180.0f), 1.0f, 0.1f, 10.0f);
	
	Vector<float> origin = Vector<float>{0.0f, 0.0f, 0.0f};
	Matrix<float> captureViews[6] = {
		utils::view(origin, Vector<float>{1, 0, 0}, Vector<float>{0, -1, 0}),
		utils::view(origin, Vector<float>{-1, 0, 0}, Vector<float>{0, -1, 0}),
		utils::view(origin, Vector<float>{0, 1, 0}, Vector<float>{0, 0, 1}),
		utils::view(origin, Vector<float>{0, -1, 0}, Vector<float>{0, 0, -1}),
		utils::view(origin, Vector<float>{0, 0, 1}, Vector<float>{0, -1, 0}),
		utils::view(origin, Vector<float>{0, 0, -1}, Vector<float>{0, -1, 0}),
	};

	Shader irradianceShader("srcs/Skybox/irradiance.vs", "srcs/Skybox/irradiance.fs");
	irradianceShader.bind();
	irradianceShader.setInt("environmentMap", 0);
	irradianceShader.setMatrix4_true("projection", captureProjection.datal());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubeMaptexture);

	glViewport(0,0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	for(size_t i = 0; i < 6; ++i) {
		irradianceShader.setMatrix4_true("view", captureViews[i].datal());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->irradianceMapId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(this->skyboxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);
};

void Skybox::generatePrefilterMap() {
	glGenTextures(1, &this->prefilterMapId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->prefilterMapId);

	int resolution = 256;
	for (size_t mip = 0; mip < 5; ++mip) {
		int mipSize = resolution >> mip;

		for (size_t face = 0; face < 6; ++face)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, GL_RGB16F, mipSize, mipSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 4);

	GLuint fbo, rbo;
	glGenFramebuffers(1, &fbo);
	glGenRenderbuffers(1, &rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	Matrix<float> captureProjection = utils::perspective(90.0f * (3.1415926f / 180.0f), 1.0f, 0.1f, 10.0f);
	
	Vector<float> origin = Vector<float>{0.0f, 0.0f, 0.0f};
	Matrix<float> captureViews[6] = {
		utils::view(origin, Vector<float>{1, 0, 0}, Vector<float>{0, -1, 0}),
		utils::view(origin, Vector<float>{-1, 0, 0}, Vector<float>{0, -1, 0}),
		utils::view(origin, Vector<float>{0, 1, 0}, Vector<float>{0, 0, 1}),
		utils::view(origin, Vector<float>{0, -1, 0}, Vector<float>{0, 0, -1}),
		utils::view(origin, Vector<float>{0, 0, 1}, Vector<float>{0, -1, 0}),
		utils::view(origin, Vector<float>{0, 0, -1}, Vector<float>{0, -1, 0}),
	};

	Shader prefilterShader("srcs/Skybox/prefilter.vs", "srcs/Skybox/prefilter.fs");
	prefilterShader.bind();
	prefilterShader.setInt("environmentMap", 0);
	prefilterShader.setMatrix4_true("projection", captureProjection.datal());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubeMaptexture);
	
	for (size_t mip = 0; mip < 5; ++mip) {
		size_t mipWidth = resolution >> mip;
		size_t mipHeight = resolution >> mip;

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / 4.0f;
		prefilterShader.setFloat("roughness", roughness);

		for (size_t i = 0; i < 6; ++i) {
			prefilterShader.setMatrix4_true("view", captureViews[i].datal());
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->prefilterMapId, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(this->skyboxVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);
};



void Skybox::draw(Matrix<float> view, Matrix<float>& projection) {

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	shaders.bind();
	shaders.setMatrix4_true("view", view.datal());
	shaders.setMatrix4_true("projection", projection.datal());
	shaders.setInt("debugMode", DEBUG_MODE);

	glBindVertexArray(this->skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMaptexture);
	glDrawArrays(GL_TRIANGLES,0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

};

Skybox::~Skybox() {
	glDeleteVertexArrays(1, &this->skyboxVAO);
    glDeleteBuffers(1, &this->skyboxVBO);
    glDeleteTextures(1, &this->cubeMaptexture);
};

