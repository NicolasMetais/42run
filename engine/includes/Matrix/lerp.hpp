#pragma once
#include <iostream>
#include <math.h>
#include "Vector.hpp"
#include "Matrix.hpp"
#include <iostream>


template <typename K>
K lerp(const K& u, const K& v, float t);

template <typename K>
typename std::enable_if<std::is_arithmetic<K>::value, K>::type
lerp_scalar(const K& u, const K&v, float t) {
	return std::fma(t, v, (1 - t) * u);
}

template <typename K>
Vector<K> lerp(const Vector<K>&u ,const Vector<K>& v, float t) {
	if (u.data.size() != v.data.size())
		throw std::invalid_argument("Vectors sizes must be the same");
	Vector<K> result;
	result.data.resize(u.data.size());
	for (size_t i = 0; i < u.data.size(); ++i)
		result.data[i] = lerp(u.data[i], v.data[i], t);
	return result;
}

template <typename K>
Matrix<K> lerp(const Matrix<K>&u ,const Matrix<K>&v, float t) {
	if (u.data.size() != v.data.size())
		throw std::invalid_argument("Matrixes sizes must be the same");
	Matrix<K> result;
	result.data.resize(u.data.size());
	for (size_t i = 0; i < u.data.size(); ++i)
	{
		if (u.data[i].size() != v.data[i].size())
			throw std::invalid_argument("Matrixes sizes must be the same");
		result.data[i].resize(u.data[i].size());
	for (size_t j = 0; j < u.data[i].size(); ++j)
		result.data[i][j] = lerp(u.data[i][j], v.data[i][j], t);
	}
	return result;
}

template <typename K>
std::complex<K> lerp(const std::complex<K>& u, const std::complex<K>& v, float t) {
    return std::complex<K>(
        lerp(u.real(), v.real(), t),
        lerp(u.imag(), v.imag(), t)
    );
}

template <typename K>
K lerp(const K&u, const K& v, float t) {
	if constexpr (std::is_arithmetic_v<K>)
		return lerp_scalar(u, v, t);
	else
		return ::lerp(u, v, t);
}