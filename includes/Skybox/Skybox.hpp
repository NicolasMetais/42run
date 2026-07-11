#pragma once
#include <iostream>
#include <Matrix/Matrix.hpp>
#include <Loader/ObjImporter.hpp>
#include <algorithm>
#include <glad/glad.h>
#include <Window.hpp>
#include <Shader.hpp>
#include <Texture/Texture.hpp>
#include <Skybox/ParseHdr.hpp>
#include <utils.hpp>
#include <Camera.hpp>

/** @brief HDR environment cubemap with irradiance and prefiltered specular maps for PBR. */
class Skybox {
	private:
		GLuint skyboxVAO, skyboxVBO;
		GLuint cubeMaptexture;    ///< Main environment cubemap texture ID.
		GLuint irradianceMapId;   ///< Diffuse irradiance cubemap for PBR ambient.
		GLuint prefilterMapId;    ///< Prefiltered specular cubemap for PBR reflections.
		std::vector<float> skyboxVertices;
		Shader shaders;
		parseHdr hdr;
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
