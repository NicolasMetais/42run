#pragma once
#include <array>
#include <Image.hpp>
#include <Text.hpp>
#include <Mat.hpp>
#include <cfloat>

#include <Matrix/Matrix.hpp>

/** @brief A single vertex with all optional attribute sets supported by the renderer. */
struct Vertex {
	Vector<float> position;
	Vector<float> normal;
	Vector<float> tangent;

	std::vector<Vector<float>> uv;    ///< One entry per UV set.
	std::vector<float> color;         ///< Raw color components.

	std::array<int, 4>   joints  = {0, 0, 0, 0};   ///< Up to 4 joint indices for skinning.
	std::array<float, 4> weights = {0.f, 0.f, 0.f, 0.f}; ///< Corresponding blend weights.
};

/** @brief A contiguous batch of vertices sharing a single material, with its own VAO/VBO/EBO. */
struct SubMesh {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint EBO = 0;

	GLenum indexType = GL_UNSIGNED_INT;
	int materialIndex = -1;

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

/** @brief All GPU-ready data for a model: submeshes, materials, textures, and bounding info. */
struct MeshData {
	std::vector<SubMesh> submeshes;
	std::vector<Mat> materials;
	std::vector<Image> images;
	std::vector<Text> textures; ///< Sampler + source pairs used by materials.

	Vector<float> min = {FLT_MAX, FLT_MAX, FLT_MAX}; ///< AABB minimum corner.
	Vector<float> max = {FLT_MIN, FLT_MIN, FLT_MIN}; ///< AABB maximum corner.
	Vector<float> center;   ///< Bounding sphere center.
	float radius = 0.0f;    ///< Bounding sphere radius.
};
