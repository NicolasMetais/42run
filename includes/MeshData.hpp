#pragma once
#include <array>
#include <Image.hpp>
#include <Text.hpp>
#include <Mat.hpp>
#include <cfloat>

#include <Matrix/Matrix.hpp>

struct Vertex {
	Vector<float> position;
	Vector<float> normal;

	Vector<float> tangent;

	std::vector<Vector<float>> uv;

	std::vector<float> color;

	std::array<int, 3> joints; //a coder plus tard
	std::array<float, 4> weights; //a coder plus tard

};

struct SubMesh { //un submesh par material pour render le mesh par type de materials
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint EBO = 0;
	const Mat* material = nullptr; //IL FAUT ADAPTER CETTE STRUCTURE A MON OBJIMPORTER

	//checker pour building d'attributes modulable openGL
	bool hasPos = false;
	bool hasNormal = false;
	bool hasUv = false;
	bool hasTangent = false;

	bool hasTexCoord = false;
	size_t texCoordCount = 0;

	bool hasColor = false;
	size_t colorCount = 0;

	bool hasJoints = false;
	size_t jointCount = 0;

	bool hasWeights = false;
	size_t weightCount = 0;
};

struct MeshData {
	std::vector<SubMesh> submeshes;
	std::vector<Mat> materials;
	std::vector<Image> images;
	std::vector<Text> textures; //liste de sampler et de source qui pointe sur un filtre et un uri


	Vector<float> min = {FLT_MAX, FLT_MAX, FLT_MAX};
	Vector<float> max = {FLT_MIN, FLT_MIN, FLT_MIN};
	Vector<float> center;
	float radius = 0.0f;
};

