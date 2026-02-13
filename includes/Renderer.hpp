#pragma once
#include <iostream>
#include <ObjImporter.hpp>
#include <TextureManager.hpp>
#include <TextureManager.hpp>
#include <Shader.hpp>
#include <algorithm>
#include <include/glad/glad.h>
#include <Window.hpp>
#include <Camera.hpp>

class Renderer {
	private:
		// GLuint shaderProgram;
		// bool transitionning = false;
		// bool transitionDir = true;

        Shader gltfShader;
        Shader objShader;
        bool useTexture = false;
		float transition = 0.0f;
		float transitionTarget = 0.0f;
		float transitionSpeed = 0.5f;

		Texture ScopTexture;
        TextureManager cache;
	public:
		Renderer();
		void rendering(Matrix<float>& mvp, MeshData& obj, Matrix<float> model, Camera& camera, float deltaTime);
		void InitObj(MeshData& obj);
		void cleanup(MeshData& obj);
		void bindTexture(int& texSlot, GLuint loc, GLuint , const Texture* texture);
		bool isTransitionning() const { return std::abs(transition - transitionTarget) > 0.001f; };
        void toggleTexture();
		void sendCommonUniforms(Shader& shader, Matrix<float>& mvp, Matrix<float>& model, Camera& camera);
		void sendMaterialUniforms(Shader& shader, const Mat* mat);
};