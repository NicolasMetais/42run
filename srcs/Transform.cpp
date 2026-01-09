#include <Transform.hpp>
#include <Matrix/Matrix.hpp>
#include <utils.hpp>

Matrix<float> Transform::getModelMatrix() {
	Matrix<float> T = utils::translation(this->position);
	Matrix<float> Rx = utils::rotationX(this->rotation.x());
	Matrix<float> Ry = utils::rotationY(this->rotation.y());
	Matrix<float> Rz = utils::rotationZ(this->rotation.z());
	Matrix<float> S = identity<float>(4);
	S[0][0] *= this->scale;
	S[1][1] *= this->scale;
	S[2][2] *= this->scale;
	return T * (Rx * Ry * Rz) * S;
};

void Transform::move(const Vector<float>& delta) {
	position.x() += delta.x();
	position.y() += delta.y();
	position.z() += delta.z();
};

void Transform::addRotation(float x, float y, float z) {
	rotation.x() += x;
	rotation.y() += y;
	rotation.z() += z;

};


