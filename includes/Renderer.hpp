#pragma once
#include <iostream>
#include <ObjImporter.hpp>
#include <TextureManager.hpp>
#include <TextureManager.hpp>
#include <Shader.hpp>
#include <algorithm>
#include <glad/glad.h>
#include <Window.hpp>
#include <utils.hpp>
#include <Camera.hpp>

/** @brief Describes a single vertex attribute for dynamic VAO construction. */
struct VertexAttribute {
	GLuint index;      ///< Shader attribute location.
	GLuint size;       ///< Number of components (e.g. 3 for a VEC3).
	GLenum type;       ///< Component GL type (e.g. GL_FLOAT).
	bool normalized;
	size_t offset;     ///< Byte offset within the vertex struct.
};

/** @brief A full vertex format description: ordered attributes and total stride. */
struct VertexLayout {
	std::vector<VertexAttribute> attributes;
	GLsizei stride;
};

/** @brief Issues OpenGL draw calls, manages shaders, and binds material uniforms. */
class Renderer {
	private:
        Shader gltfShader;
        Shader objShader;
        Shader debug;

		/** @brief Uploads MVP, model matrix, and camera uniforms shared by all shaders. */
		void sendCommonUniforms(Shader& shader, Matrix<float>& mvp, Matrix<float>& model, Camera& camera);
		/** @brief Binds textures and uploads PBR/Phong material parameters for a submesh. */
		void sendMaterialUniforms(Shader& shader, const Mat* mat, SubMesh& mesh, GLuint skyboxId, GLuint prefilterMapId);
		/** @brief Issues the indexed or array draw call for a submesh. */
		void draw(SubMesh& mesh);
	public:
		Renderer();
		/**
		 * @brief Renders all submeshes of @p obj with the given matrices and environment maps.
		 * @param mvp           Combined model-view-projection matrix.
		 * @param obj           Mesh data to render.
		 * @param model         Model (world) matrix for lighting calculations.
		 * @param camera        Camera providing position and view data.
		 * @param skyboxId      Cubemap texture ID for reflections.
		 * @param prefilterMapId Prefiltered environment map for PBR specular.
		 * @param jointMats     Optional joint matrices for skeletal animation.
		 */
		void rendering(Matrix<float>& mvp, MeshData& obj, Matrix<float> model, Camera& camera, GLuint skyboxId, GLuint prefilterMapId, const std::vector<Matrix<float>>* jointMats = nullptr);
		/** @brief Uploads vertex data to the GPU and sets up the VAO for a submesh. */
		void InitMesh(SubMesh& subMesh);
		/** @brief Deletes all OpenGL buffers and textures owned by @p obj. */
		void cleanup(MeshData& obj);
		/** @brief Binds a texture to a shader sampler slot, incrementing the slot counter. */
		void bindTexture(int& texSlot, GLuint loc, GLuint, const Texture* texture);
};
