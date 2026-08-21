template <typename K>
Matrix<K>::Matrix(std::initializer_list<std::initializer_list<K>> list) {
	rows = list.size();
	cols = list.begin()->size();
	data.reserve(rows * cols);
	for (auto& row : list)
		for(auto& val : row)
			data.push_back(val);
};

template <typename K>
void Matrix<K>::add(const Matrix<K>& m) {
	if (this->shape() != m.shape())
		throw std::invalid_argument("Matrice should have same dimensions");
	for(size_t i = 0; i < this->rows; ++i)
		for(size_t j = 0; j < this->cols; ++j)
			data[i * cols + j] += m.data[i * cols + j];
}

template <typename K>
void Matrix<K>::sub(const Matrix<K>& m) {
	if (this->shape() != m.shape())
		throw std::invalid_argument("Matrice should have same dimensions");
	for(size_t i = 0; i < this->rows; ++i)
		for(size_t j = 0; j < this->cols; ++j)
			data[i * cols + j] -= m.data[i * cols + j];
}

template <typename K>
void Matrix<K>::scl(K val) {
	for(size_t i = 0; i < this->rows * this->cols; ++i)
		data[i] *= val;
}

template <typename K>
std::pair<size_t, size_t> Matrix<K>::shape() const {
	return {this->rows, this->cols};
}

template <typename K>
size_t Matrix<K>::size() const {
	return data.size();
}

template <typename K>
bool Matrix<K>::empty() const {
	return data.empty();
}


template <typename K>
Vector<K> Matrix<K>::mul_vec(const Vector<K>& vec) const {
	if (empty() || vec.size() != this->cols)
		throw std::invalid_argument("Vector size does not match number of matrix columns");
	Vector<K> res;
	res.data.resize(rows, 0);
	for (size_t i = 0; i < this->rows; ++i) {
		for(size_t j = 0; j < this->cols; ++j) {
			res[i] += data[i * cols + j] * vec[j];
		}
	}
	return res;
}

template <typename K>
Matrix<K> Matrix<K>::mul_mat(const Matrix<K>& vec) const {
	if (this->cols != vec.rows)
		throw std::invalid_argument("Matrix sizes error");
	Matrix<K> res;
	res.rows = this->rows;
	res.cols = vec.cols;
	res.data.assign(res.rows * res.cols, K{0});
	for (size_t i = 0; i < this->rows; ++i) {
		for(size_t j = 0; j < res.cols; ++j) {
			for (size_t k = 0; k < this->cols; ++k)
				res.data[i * res.cols + j] += data[i * cols + k] * vec.data[k * res.cols + j];
		}
	}
	return res;
}

template <typename K>
bool Matrix<K>::square() const {
	if (this->rows == this->cols)
		return true;
	return false;
}


template <typename K>
K Matrix<K>::trace() const {
	if (!square())
		throw std::invalid_argument("Matrix must be a square");
	K res = 0;
	for (size_t i = 0; i < this->rows; ++i)
		res += data[i * this->cols + i];
	return res;
}

template <typename K>
Matrix<K> Matrix<K>::transpose() const {
	Matrix<K> transpose;
	transpose.data.resize(this->cols * this->rows);
	transpose.rows = cols;
	transpose.cols = rows;
	for(size_t i = 0; i < this->rows; ++i)
		for(size_t j = 0; j < this->cols; ++j)
			transpose.data[j * transpose.cols + i] = data[i * cols + j];
	return transpose;
}

template <typename K>
Matrix<K> Matrix<K>::row_echelon() const {
	Matrix<K> res = *this;
	const size_t rows = res.rows;
	const size_t cols = res.cols;
	size_t lead = 0;
	for (size_t r = 0; r < rows; ++r) {
		if (lead >= cols)
			break ;
		size_t i = r;
		while (i < rows && isZero(res.data[i * cols + lead]))
			++i;
		if (i == rows) {
			++lead;
			--r;
			continue;
		}
		if (i != r)
			for (size_t c = 0; c < cols; c++)
				std::swap(res.data[i * cols + c], res.data[r * cols + c]);
		
		K pivot = res[r * cols + lead];
		for (size_t j = lead; j < cols; ++j)
			res[r * cols +j] /= pivot;

		for (size_t j = r + 1; j < rows; ++j) {
			K facteur = res.data[j * cols + lead];
			if (isZero(facteur)) {
				res.data[j * cols + lead] = static_cast<K>(0);
				continue ;
			}
			for(size_t k = lead; k < cols; ++k)
				res.data[j * cols + k] -= facteur * res.data[r * cols + k];
			res.data[j * cols + lead] = static_cast<K>(0);
		}
	}
	return res;
}

template <typename K>
Matrix<K> Matrix<K>::reduced_row_echelon() const {
	Matrix<K> res = row_echelon();
	//RREF
	for (size_t i = 0; i < res.rows; ++i) {
		K pivot = static_cast<K>(0);
		size_t pivot_col = 0;
		for (size_t j = 0; j < res.cols; ++j) { //je cherche le pivot
			if (!isZero(res.data[i * res.cols + j])) {
				pivot = res.data[i * res.cols + j];
				pivot_col = j;
				break ;
			}
		}
		if (!isZero(pivot)) {  //je normalise tout ce qui est a droite du pivot
			for (size_t j = pivot_col; j < res.cols; ++j)
				res.data[i * res.cols + j] /= pivot;
		}
	}
	for (int i = static_cast<int>(res.rows) - 1; i >= 0; --i) //je vais voir au dessus des pivot pour re-calculer la matrice
	{	
		size_t pivot_col = 0;
		while (pivot_col < res.cols && isZero(res[i * res.cols + pivot_col]))
			++pivot_col;
		if (pivot_col == res.cols) //pas de pivots
			continue ;
		for (int j = i - 1; j >= 0; --j) {
			K facteur = res[j * res.cols + pivot_col];
			if (!isZero(facteur)) {
				for(size_t k = pivot_col; k < res.cols; ++k)
					res[j * res.cols + k] -= facteur * res[i * res.cols + k];
			}
		}
	}
	return res;
}

template <typename K>
Matrix<K> Matrix<K>::row_echelon_det(size_t *swap) const {
	Matrix<K> res = *this;
	size_t lead = 0;
	for (size_t r = 0; r < res.rows; ++r) {
		if (lead >= res.cols)
			break ;
		size_t i = r;
		while (i < res.rows && isZero(res[i * res.cols + lead]))
			++i;
		if (i == res.rows) {
			++lead;
			--r;
			continue;
		}
		if (i != r)
		{
			for (size_t c = 0; c < res.cols; c++)
				std::swap(res.data[i * res.cols + c], res.data[r * res.cols + c]);
			(*swap)++;
		}
		K pivot = res[r * res.cols + lead];
		for (size_t j = r + 1; j < res.rows; ++j) {
			K facteur = res[j * res.cols + lead] / pivot;
			for(size_t k = lead; k < res.cols; ++k)
				res[j * res.cols + k] -= facteur * res[r * res.cols + k];
			res[j * res.cols + lead] = static_cast<K>(0);
		}
	}
	return res;
}

template <typename K>
Matrix<K> identity(size_t n) {
	Matrix<K> res;
	res.rows = n;
	res.cols = n;
	res.data.assign(n * n, K{0});
	for (size_t i = 0; i < n; ++i)
		res.data[i * n + i] = K{1};
	return res;
};


template <typename K>
K Matrix<K>::determinant() const {
	K det = 1;
	size_t swap = 0;
	if (this->rows != this->cols || this->rows > 4)
		throw std::invalid_argument("This calculation only apply on a square matrix of 4x4 or less");
	Matrix<K> res = row_echelon_det(&swap);
	for (size_t i = 0; i < this->rows; i++) {
		if (isZero(res.data[i * cols + i])) 
		{
			det = 0;
			break ;
		}
		det *= res.data[i * cols + i];
	}
	if (swap % 2 != 0)
		det = -det;
	return det;
};

template <typename K> 
Matrix<K> Matrix<K>::submatrix(size_t r, size_t c) const {
	if (this->rows <= 1 || this->cols <= 1)
    	throw std::invalid_argument("Cannot create submatrix for 1xN or Nx1");
	Matrix<K> res;
	res.data.resize(this->rows - 1);
	for (size_t i = 0; i < this->rows - 1; ++i)
		res.data[i].resize(this->cols - 1);
	size_t row_res = 0;
	for (size_t i = 0; i < this->rows; ++i) {
		if (i == r)
			continue ;
		size_t col_res = 0;
		for(size_t j = 0; j < this->cols; ++j) {
			if (j == c)
				continue ;
			res[row_res][col_res++] = (*this)[i][j];
		}
		row_res++;
	}
	return res;
};


template <typename K>
Matrix<K> Matrix<K>::inverse() const {
	if (!square())
		throw std::invalid_argument("Matrix must be a square");
	Matrix<K> ident = identity<K>(this->rows);
	Matrix<K> res = *this;
	for (size_t i = 0; i < res.rows; ++i) {
		K pivot = res[i * res.cols + i]; //recherche de pivot
		if (isZero(pivot)) // si le pivot est nul j'en cherche un non nul et je swap la ligne
		{
			bool found = false;
			for (size_t j = i + 1; j < res.rows; ++j){
				if (!isZero(res[j * res.cols + i])) {
					std::swap(res[i], res[j]);
					std::swap(ident[i], ident[j]);
					pivot = res[i * cols + i];
					found = true;
					break ;
				}
			}
			if (!found)
				throw std::invalid_argument("Singular Matrix");
		}
		for (size_t j = 0; j < res.rows; ++j) { // je normalise toute la ligne pour obtenir des 1.0 en diagonale
			res[i][j] /= pivot;
			ident[i][j] /= pivot;
		}
		for (size_t j = 0; j < res.rows; ++j) { //je vide toute la matrice sauf les pivot
			if (j == i)
				continue ;
			K factor = res[j][i];
			for (size_t k = 0; k < res.rows; ++k) {
				res[j * res.cols + k] -= factor * res[i * res.cols + k];
				ident[j * res.cols + k] -= factor * ident[i * res.cols + k];
			}
		}
	}
	return ident;
};

template <typename K>
size_t Matrix<K>::rank() const {
	Matrix<K> RREF;
	size_t rank = 0;
	RREF = reduced_row_echelon();
	for (size_t i = 0; i < RREF.rows; ++i) {
		for (size_t j = 0; j < RREF.cols; ++j) { //Je parcourt la RREF jusqu'a trouver qqch different de 0 qui sera automatiquement mon pivot. Si je trouve qqch je change de ligne
			if (!isZero(RREF[i  * RREF.cols + j])) {
				rank++;
				break ;
			}
		}
	}
	return rank;
};

template <typename K>
Matrix<K> projection(float fov, float ratio, float near, float far) {
	K f = K(1) / std::tan(K(fov) / K(2));
	Matrix<K> proj({
		{f / ratio, K(0), K(0), K(0)},
		{K(0), f, K(0), K(0)},
		{K(0), K(0), -(far + near) / (far - near), K(1)},
		{K(0), K(0), -(2 * far * near) / (far - near), K(0)}
	});
	return proj;
};

template <typename K>
Matrix<K> Matrix<K>::operator*(K f) const {
	Matrix<K> res = *this;
	res.scl(f);
	return res;
};

template <typename K>
Matrix<K> Matrix<K>::operator*(const Matrix<K>& m) const {
	return mul_mat(m);
};

template <typename K>
Vector<K> Matrix<K>::operator*(const Vector<K>& v) const {
	return mul_vec(v);
};
