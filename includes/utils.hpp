#pragma once
#include <iostream>
#include <Matrix/Matrix.hpp>
#include <MeshData.hpp>
#include <TextureManager.hpp>
#include <Node.hpp>

namespace utils {
	Matrix<float> rotationX(float x);
	Matrix<float> rotationY(float y);
	Matrix<float> rotationZ(float z);
	Matrix<float> perspective(float fov, float aspect, float near, float far);
	Matrix<float> view(Vector<float> pos, Vector<float> forward, Vector<float> up);
	Matrix<float> translation(const Vector<float>& position);
	float Todegres(float rad);
	float ToRad(float rad);

	Matrix<float> nodeLocalMatrix(const Node& node);

	void smoothNormals(MeshData& meshdata);
	void prepareMats(MeshData& meshdata, TextureManager& texMng);
	std::string resolveUri(MeshData& meshdata, int index);

}