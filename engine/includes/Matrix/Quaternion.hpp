#pragma once
#include "Vector.hpp"
#include <iostream>
#include <cmath>

template <typename K>
struct Vector;

template <typename K>
struct Quaternion {
	K w, x, y, z;
	Quaternion() : w(1), x(0), y(0), z(0) {}
	Quaternion(K w, K x, K y, K z): w(w), x(x), y(y), z(z) {};
	Quaternion(K angleDeg, const Vector<K>& axis);

	void normalize();
	Quaternion<K> conjugate() const;
	Quaternion<K> inverse() const;

	Quaternion<K> operator*(const Quaternion& r) const;
	Quaternion<K> Rotate(float angle, const Vector<K>& axis) const;
	Vector<K> operator*(const Vector<K>& v) const;
	friend Quaternion<K> operator*(const Vector<K>& v, const Quaternion<K>& q) {
		    Quaternion<K> qv(0, v.x(), v.y(), v.z());
   		return qv * q;
	};

	// Matrix<K> toMatrix3() const;
	// Matrix<K> toMatrix4() const;
};


#include "Quaternion.tpp"