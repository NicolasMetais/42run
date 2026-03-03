#pragma once
#include <iostream>
#include <ObjImporter.hpp>
#include <TextureManager.hpp>
#include <TextureManager.hpp>
#include <Shader.hpp>
#include <algorithm>
#include <include/glad/glad.h>
#include <Window.hpp>
#include <utils.hpp>
#include <Camera.hpp>

struct VertexAttribute { //description d'un attribut pour gestion dynamique
	GLuint index;
	GLuint size;
	GLenum type;
	bool normalized;
	size_t offset;
};

struct VertexLayout {
	std::vector<VertexAttribute> attributes;
	GLsizei stride;
};

class Renderer {
	private:
        Shader gltfShader;
        Shader objShader;
        Shader debug;

		void sendCommonUniforms(Shader& shader, Matrix<float>& mvp, Matrix<float>& model, Camera& camera);
		void sendMaterialUniforms(Shader& shader, const Mat* mat, SubMesh& mesh, GLuint skyboxId);
		void draw(SubMesh& mesh);
	public:
		Renderer();
		void rendering(Matrix<float>& mvp, MeshData& obj, Matrix<float> model, Camera& camera, GLuint skyboxId);
		void InitMesh(SubMesh& subMesh);
		void cleanup(MeshData& obj);
		void bindTexture(int& texSlot, GLuint loc, GLuint , const Texture* texture);


};