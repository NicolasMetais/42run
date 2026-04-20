#pragma once
#include <iostream>
#include <Matrix/Matrix.hpp>
#include <ObjImporter.hpp>
#include <algorithm>
#include <include/glad/glad.h>
#include <Window.hpp>
#include <Shader.hpp>
#include <Texture.hpp>
#include <utils.hpp>
#include <Camera.hpp>

struct SkyboxFace {
	std::string name;
	Texture tex;
};

class Skybox {
	private:
		GLuint skyboxVAO, skyboxVBO;
		GLuint cubeMaptexture;
		GLuint irradianceMapId;
		std::vector<SkyboxFace> TextureList;
		std::vector<float> skyboxVertices;
		Shader shaders;
		public:
		Skybox();
		void draw(Matrix<float> view, Matrix<float>& projection);
		~Skybox();
	public:
		GLuint getSkyboxId() { return this->cubeMaptexture; };
		GLuint getIrradianceMapId() { return this->irradianceMapId; };

		void generateIrradianceMap();
};