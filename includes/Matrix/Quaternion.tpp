template <typename K>
Quaternion<K>::Quaternion(K angleDeg, const Vector<K>& axis) {
	K rad = angleDeg * M_PI / 180.0;
	K s = std::sin(rad / 2.0);
	w = std::cos(rad / 2.0);
	x = axis.x() * s;
	y = axis.y() * s;
	z = axis.z() * s;
};

template <typename K>
void Quaternion<K>::normalize() {
	K norm = sqrt(x * x + y * y + z * z + w * w);
	if (norm != 0) {
		x/= norm;
		y /= norm;
		z /= norm;
		w /= norm;
	}
};

template <typename K>
Quaternion<K> Quaternion<K>::conjugate() const {
	Quaternion ret(w, -x, -y, -z);
	return ret;
};

template <typename K>
Quaternion<K> Quaternion<K>::inverse() const {
    return conjugate(); //
}

template <typename K>
Quaternion<K> Quaternion<K>::operator*(const Quaternion& r) const {
	K nw = w*r.w - x*r.x - y*r.y - z*r.z;
	K nx = w*r.x + x*r.w + y*r.z - z*r.y;
	K ny = w*r.y - x*r.z + y*r.w + z*r.x;
	K nz = w*r.z + x*r.y - y*r.x + z*r.w;

	Quaternion ret(nw, nx, ny, nz);
	return ret;
};

template <typename K>
Vector<K> Quaternion<K>::operator*(const Vector<K>& v) const {
	Quaternion<K> vq(0, v.x(), v.y(), v.z());
	Quaternion<K> rq = (*this) * vq * conjugate();
	return Vector<K>{rq.x, rq.y, rq.z};
};