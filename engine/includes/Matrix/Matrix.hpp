#pragma once
#include <iostream>
#include <vector>
#include "Vector.hpp"
#include <math.h>
#include <complex>

template <typename K>
struct Matrix {
	std::vector<K> data;
	size_t rows;
	size_t cols;

	std::pair<size_t, size_t> shape() const;
	void add(const Matrix<K>& m);
	void sub(const Matrix<K>& m);
	void scl(K val);
	bool square() const;
	K trace() const;
	size_t size() const;
	bool empty() const;
	Vector<K> mul_vec(const Vector<K>& vec) const;
	Matrix<K> mul_mat(const Matrix<K>& vec) const;
	Matrix<K> transpose() const;
	Matrix<K> row_echelon() const;
	Matrix<K> row_echelon_det(size_t *swap) const;
	Matrix<K> reduced_row_echelon() const;
	K determinant() const;
	K* datal() { return data.data(); }
	Matrix<K> inverse() const;
	size_t rank() const;
	Matrix<K> submatrix(size_t r, size_t c) const;
	K* operator[](size_t i) { return data.data() + i * cols; }
	const K* operator[](size_t i) const { return data.data() + i * cols; }
	Matrix<K> operator*(K f) const;
	Vector<K> operator*(const Vector<K>& v) const;
	Matrix<K> operator*(const Matrix<K>& m) const;
	Matrix() = default;
	Matrix(std::initializer_list<std::initializer_list<K>> list);
	friend std::ostream& operator<<(std::ostream& os, const Matrix<K>& m) {
		for (size_t i = 0; i < m.rows; ++i)
		{
			os << "{";

			for (size_t j = 0; j < m.cols; ++j)
			{
				os << m.data[i * m.cols + j];
				if (j < m.cols - 1)
					os << ", ";
			}
			os << "} ";
		}
		return os;
	}
};

template <typename K>
Matrix<K> identity(size_t n);

template <typename K>
Matrix<K> projection(float fov, float ratio, float near, float far);

constexpr double EPS = 1e-16;

template <typename K>
bool isZero(const K& x) {
	return std::abs(x) < EPS;
}

#include "Matrix.tpp"
