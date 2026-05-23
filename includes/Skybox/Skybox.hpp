#pragma once
#include <iostream>
#include <Matrix/Matrix.hpp>
#include <Loader/ObjImporter.hpp>
#include <algorithm>
#include <glad/glad.h>
#include <Window.hpp>
#include <Shader.hpp>
#include <Texture/Texture.hpp>
#include <utils.hpp>
#include <Camera.hpp>

/** @brief One face of the skybox cubemap with its associated texture. */
struct SkyboxFace {
	std::string name;
	Texture tex;
};

/** @brief HDR environment cubemap with irradiance and prefiltered specular maps for PBR. */
class Skybox {
	private:
		GLuint skyboxVAO, skyboxVBO;
		GLuint cubeMaptexture;    ///< Main environment cubemap texture ID.
		GLuint irradianceMapId;   ///< Diffuse irradiance cubemap for PBR ambient.
		GLuint prefilterMapId;    ///< Prefiltered specular cubemap for PBR reflections.
		std::vector<SkyboxFace> TextureList;
		std::vector<float> skyboxVertices;
		Shader shaders;
	public:
		Skybox();
		/** @brief Renders the skybox cube using a view matrix with no translation. */
		void draw(Matrix<float> view, Matrix<float>& projection);
		~Skybox();

		GLuint getSkyboxId() { return this->cubeMaptexture; };
		GLuint getIrradianceMapId() { return this->irradianceMapId; };
		GLuint getPrefilterMapId() { return this->prefilterMapId; };

		/** @brief Convolves the environment cubemap into a diffuse irradiance map. */
		void generateIrradianceMap();
		/** @brief Generates mip-level prefiltered specular maps at varying roughness levels. */
		void generatePrefilterMap();
};
