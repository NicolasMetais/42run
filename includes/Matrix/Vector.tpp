template <typename K>
Vector<K>::Vector(size_t n) : data(n) {};

template <typename K>
void Vector<K>::add(const Vector<K>& v) {
	if (size() != v.size())
		throw std::invalid_argument("Vectors must be the same size");
	for(size_t i = 0; i < size(); ++i)
		data[i] += v.data[i];
}

template <typename K>
void Vector<K>::sub(const Vector<K>& v) {
	if (size() != v.size())
		throw std::invalid_argument("Vectors must be the same size");
	for(size_t i = 0; i < size(); ++i)
		data[i] -= v.data[i];
}

template <typename K>
void Vector<K>::scl(const K& val) {
	for(size_t i = 0; i < size(); ++i)
		data[i] *= val;
}

template <typename K>
Vector<K> linear_combination(const std::vector<Vector<K>>& u, const std::vector<K>& coefs) {
	// if (u.size() != coefs.size())
	// 	throw std::invalid_argument("The vectors and the coefs sizes must be the same");
	if (u.empty())
        throw std::invalid_argument("Empty vector list");
	size_t len = u[0].size();
	Vector<K> result;
	result.data.resize(len, K{0});
	for (size_t i = 0; i < u.size(); ++i)
	{
		// if (u[i].size() != len)
		// 	throw std::invalid_argument("The vectors must all be the same size");
		for (size_t j = 0; j < len; j++)
			result.data[j] += coefs[i] * u[i].data[j];
	}
	return result;
}

template <typename K>
K Vector<K>::dot(const Vector<K>& v) const{
	if (size() != v.size())
		throw std::invalid_argument("The vectors sizes must be the same");
	K result{};
	for (size_t i = 0; i < data.size(); ++i)
		result += data[i] * v.data[i];
	return result;
}

template <typename K> //norm taxicab/Manhattan, distance case par case
float Vector<K>::norm_1() const{
	float norm = 0;
	for (auto& i : data)
		norm += std::abs(i);
	return norm;
}

template <typename K> //norm euclidienne, pythagore "vol d'oiseau"
float Vector<K>::norm() const{
	float norm = 0.0f;
	for (auto const& v : data)
		norm += std::pow(std::abs(v), 2.0f);
	return std::pow(norm, 0.5f);
}

template <typename K> //norm supremum, valeur max du vecteur
float Vector<K>::norm_inf() const{
	float max = 0;
	for (auto& i : data)
		max = std::max(max, std::abs(i));
	return max;
}

template <typename K>
size_t Vector<K>::size() const {
	return data.size();
}

template <typename K>
bool Vector<K>::empty() const {
	return data.empty();
}

template <typename K>
float angle_cos(const Vector<K>&u, const Vector<K>&v) {
	if (u.size() != v.size())
		throw std::invalid_argument("The vectors and the coefs sizes must be the same");
	if (u.empty())
        throw std::invalid_argument("Empty vectors");
	return std::real(u.dot(v)) / (u.norm() * v.norm());
}

template <typename K>
Vector<K> cross_product(const Vector<K>&u, const Vector<K>&v) {
	if (u.size() != 3 ||  v.size() != 3)
		throw std::invalid_argument("The vectors must be of size 3");
	Vector<K> crossed = {K(0), K(0), K(0)};
	crossed[0] = u[1] * v[2] - u[2] * v[1];
	crossed[1] = u[2] * v[0] - u[0] * v[2]; 
	crossed[2] = u[0] * v[1] - u[1] * v[0]; 
	return crossed;
}

template <typename K>
Vector<K> Vector<K>::operator*(K f) const {
	Vector<K> res;
	res.data.resize(size());
	for(size_t i = 0; i < res.size(); ++i)
		res[i] = data[i] * f;
	return res;
};

template <typename K>
Vector<K> Vector<K>::operator/(K f) const {
	if (f == static_cast<K>(0))
		throw std::invalid_argument("I can't divide by 0 bruh");
	Vector<K> res(size());
	for(size_t i = 0; i < res.size(); ++i)
		res[i] = data[i] / f;
	return res;
};

template <typename K>
Vector<K> Vector<K>::operator+(const Vector<K>& v) const {
	if (size() != v.size())
		throw std::invalid_argument("Vector size do not match");
	Vector<K> res(size());
	for(size_t i = 0; i < res.size(); ++i)
		res[i] = data[i] + v[i];
	return res;
};

template <typename K>
Vector<K> Vector<K>::operator-(const Vector<K>& v) const {
	if (size() != v.size())
		throw std::invalid_argument("Vector size do not match");
	Vector<K> res;
	res.data.resize(size());
	for(size_t i = 0; i < res.size(); ++i)
		res[i] = data[i] - v[i];
	return res;
};

template <typename K>
Vector<K>& Vector<K>::operator+=(const Vector<K>& v) {
	add(v);
	return *this;
};

template <typename K>
Vector<K>& Vector<K>::operator+=(const K& f)
{
	for(size_t i = 0; i < size(); ++i) {
		data[i] += f;
	}
	return *this;
};

template <typename K>
Vector<K>& Vector<K>::operator-=(const Vector<K>& v) {
	sub(v);
	return *this;
};

template <typename K>
Vector<K>& Vector<K>::operator-=(const K& f)
{
	for(size_t i = 0; i < size(); ++i) {
		data[i] -= f;
	}
	return *this;
};

template <typename K>
Vector<K>& Vector<K>::operator*=(const Vector<K>& v) {
	if (size() != v.size())
		throw std::invalid_argument("Vector size do not match");
	for(size_t i = 0; i < size(); ++i) {
		data[i] *= v[i];
	}
	return *this;
};

template <typename K>
Vector<K>& Vector<K>::operator*=(const K& f)
{
	scl(f);
	return *this;
};

template <typename K>
Vector<K>& Vector<K>::operator/=(const Vector<K>& v) {
	if (size() != v.size())
		throw std::invalid_argument("Vector size do not match");
	for(size_t i = 0; i < size(); ++i) {
		if (v[i] == static_cast<K>(0))
			throw std::invalid_argument("I can't divide by 0 bruh");
		data[i] /= v[i];
	}
	return *this;
};

template <typename K>
Vector<K>& Vector<K>::operator/=(const K& f)
{
	if (f == static_cast<K>(0))
		throw std::invalid_argument("I can't divide by 0 bruh");
	for(size_t i = 0; i < size(); ++i) {
		data[i] /= f;
	}
	return *this;
};

template <typename K>
K& Vector<K>::x() {
	if (size() < 1)
		throw std::invalid_argument("Vector size must be >= 1");
	return data[0];
};

template <typename K>
const K& Vector<K>::x() const {
	if (size() < 1)
		throw std::invalid_argument("Vector size must be >= 1");
	return data[0];
};

template <typename K>
K& Vector<K>::y() {
	if (size() < 2)
		throw std::invalid_argument("Vector size must be >= 2");
	return data[1];
};

template <typename K>
const K& Vector<K>::y() const {
	if (size() < 2)
		throw std::invalid_argument("Vector size must be >= 2");
	return data[1];
};

template <typename K>
K& Vector<K>::z() {
	if (size() < 3)
		throw std::invalid_argument("Vector size must be >= 3");
	return data[2];
};

template <typename K>
const K& Vector<K>::z() const {
	if (size() < 3)
		throw std::invalid_argument("Vector size must be >= 3");
	return data[2];
};

template <typename K>
K& Vector<K>::w() {
	if (size() < 4)
		throw std::invalid_argument("Vector size must be >= 4");
	return data[3];
};

template <typename K>
const K& Vector<K>::w() const {
	if (size() < 4)
		throw std::invalid_argument("Vector size must be >= 4");
	return data[3];
};

template <typename K>
Vector<K> Vector<K>::normalize() const {
	float n = norm();
	Vector<K>res;
	res.data.resize(size());
	if (n == 0.0f) {
		for (size_t i = 0; i < size(); ++i) {
			res[i] = 0.0f;
		}
		return res;
	}
	for (size_t i = 0; i < size(); ++i) {
		res[i] = data[i] / n;
	}
	return res;
};

template <typename K>
void Vector<K>::Rotate(float angle, const Vector<K>& axis) {
	if (size() < 3 || axis.size() < 3)
		return ;
	Quaternion<K> Rotation(angle, axis);

	Quaternion<K> conj = Rotation.conjugate();

	Quaternion<K> W = Rotation * (*this) * conj;

	x() = W.x;
	y() = W.y;
	z() = W.z;
	// Quaternion<K> q(angle, axis.normalize());
	// q.normalize();
	// Vector<K> v{data[0], data[1], data[2]};
	// Vector<K> rotated = q * v;
	// data[0] = rotated[0];
	// data[1] = rotated[1];
	// data[2] = rotated[2];
};

template <typename K>
Vector<K> Vector<K>::operator-() const{
	Vector<K>res(size());
	for (size_t i = 0; i < size(); ++i)
		res[i] = -data[i];
	return res;
};
