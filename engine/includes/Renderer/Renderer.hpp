#pragma once
#include <iostream>
#include <unordered_set>
#include <Loader/ObjImporter.hpp>
#include <Loader/LoadedModel.hpp>
#include <Texture/TextureManager.hpp>
#include <Texture/TextureManager.hpp>
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

/** @brief Rendu en deux passes : les opaques/MASK d'abord, puis les materiaux
 *  BLEND apres la skybox, tries loin -> proche, sans ecriture de profondeur. */
enum class RenderPass { Opaque, Transparent };

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
		void draw(SubMesh& mesh, const Mat* mat);
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
		 * @param pass          Passe courante : seuls les submeshes du bon alphaMode sont dessines.
		 */
		void rendering(Matrix<float>& mvp, MeshData& obj, Matrix<float> model, Camera& camera, GLuint skyboxId, GLuint prefilterMapId, const std::vector<Matrix<float>>* jointMats = nullptr, RenderPass pass = RenderPass::Opaque);
		/**
		 * @brief Recursively renders a glTF node and its children, composing each node's local
		 * transform against its parent. Generic scene-graph traversal, reused for anything from
		 * full 3D world entities to a single small icon rendered with its own view/projection.
		 */
		void renderNode(LoadedModel& lm, int nodeIdx, const Matrix<float>& parentWorld, const Matrix<float>& view, const Matrix<float>& projection, const std::unordered_set<int>& hiddenNodes, RenderPass pass, Camera& camera, GLuint skyboxId, GLuint prefilterMapId);
		/** @brief Coupe le blending pour la passe opaque (les pixels MASK restent pleins). */
		void beginOpaquePass();
		/** @brief Active le blending et gele l'ecriture de profondeur pour les transparents. */
		void beginTransparentPass();
		/** @brief Retablit l'ecriture de profondeur. Le blend reste ON : l'UI et la skybox comptent dessus. */
		void endTransparentPass();
		/** @brief Uploads vertex data to the GPU and sets up the VAO for a submesh. */
		void InitMesh(SubMesh& subMesh);
		/** @brief Deletes all OpenGL buffers and textures owned by @p obj. */
		void cleanup(MeshData& obj);
		/** @brief Binds a texture to a shader sampler slot, incrementing the slot counter. */
		void bindTexture(int& texSlot, GLuint loc, GLuint, const Texture* texture);
};
