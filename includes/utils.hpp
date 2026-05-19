#pragma once
#include <iostream>
#include <Matrix/Matrix.hpp>
#include <MeshData.hpp>
#include <TextureManager.hpp>
#include <Loader/Node.hpp>

/** @brief Math and scene utility functions used throughout the renderer. */
namespace utils {
	/** @brief Returns a rotation matrix around the X axis by @p x radians. */
	Matrix<float> rotationX(float x);
	/** @brief Returns a rotation matrix around the Y axis by @p y radians. */
	Matrix<float> rotationY(float y);
	/** @brief Returns a rotation matrix around the Z axis by @p z radians. */
	Matrix<float> rotationZ(float z);
	/** @brief Builds a perspective projection matrix. */
	Matrix<float> perspective(float fov, float aspect, float near, float far);
	/** @brief Builds a look-at view matrix from position, forward direction and up vector. */
	Matrix<float> view(Vector<float> pos, Vector<float> forward, Vector<float> up);
	/** @brief Returns a translation matrix for @p position. */
	Matrix<float> translation(const Vector<float>& position);
	/** @brief Converts radians to degrees. */
	float Todegres(float rad);
	/** @brief Converts degrees to radians. */
	float ToRad(float rad);

	/** @brief Computes the local transform matrix for a GLTF node from its TRS or matrix field. */
	Matrix<float> nodeLocalMatrix(const Node& node);

	/** @brief Averages per-face normals into smooth per-vertex normals for the meshdata. */
	void smoothNormals(MeshData& meshdata);
	/** @brief Resolves texture URIs and uploads them to the GPU via the TextureManager. */
	void prepareMats(MeshData& meshdata, TextureManager& texMng);
	/** @brief Resolves the file URI of the texture at @p index within the meshdata. */
	std::string resolveUri(MeshData& meshdata, int index);
}
