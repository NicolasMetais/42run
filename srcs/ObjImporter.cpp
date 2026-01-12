#include <ObjImporter.hpp>

ObjImporter::FaceVertex ObjImporter::parseFaceElement(const std::string& part)
{
	FaceVertex fv { -1, -1, -1 };
	int i = 0;
	std::stringstream ss(part);
	std::string token;
	while (std::getline(ss, token, '/')) {
		if (!token.empty()) {
			int value = std::stoi(token);
			if (i == 0) 
			{
				if (value < 0)
					fv.v = v.size() + value;
				else
					fv.v = value - 1;
			}
			if (i == 1)
			{
				if (value < 0)
					fv.vt = vt.size() + value;
				else
					fv.vt = value - 1;
			}
			if (i == 2)
			{
				if (value < 0)
					fv.vn = vn.size() + value;
				else
					fv.vn = value - 1;
			}
		}
		i++;
	}
	return fv;
}


std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\r\n");
	if (first == std::string::npos)
		return "";
	size_t last = str.find_last_not_of(" \t\r\n");
	return str.substr(first, last - first + 1);
}

void ObjImporter::loadMtlFile(const std::string& fileName) {
	std::ifstream file;
	file.open("resources/" + fileName);
	if (!file.is_open())
		throw std::runtime_error("Error: cannot open mtl file");
	std::string line;
	Material current;
	while (std::getline(file, line)) {
		line = trim(line);
		if (line.empty() || line[0] == '#')
			continue ;
		std::stringstream ss(line);
		std::string token;
		ss >> token;
		if (token == "newmtl")
		{
			if (!current.getName().empty())
				materials[current.getName()] = current;
			current = Material();
			std::string name;
			ss >> name;
			current.setName(name);
		}
		else if (token == "Ka") {
			float r, g, b;
			ss >> r >> g >> b;
			current.setKa({r, g, b});
		}
		else if (token == "Kd") {
			float r, g, b;
			ss >> r >> g >> b;
			current.setKd({r, g, b});
		}
		else if (token == "Ks") {
			float r, g, b;
			ss >> r >> g >> b;
			current.setKs({r, g, b});
		}
		else if (token == "Ns") {
			float ns;
			ss >> ns;
			current.setNs(ns);
		}
		else if (token == "d") {
			float d;
			ss >> d;
			current.setd(d);
		}
		else if (token == "illum") {
			int illum;
			ss >> illum;
			current.setIllum(illum);
		}
		else if (token == "bump" || token.rfind("map_", 0) == 0) {
			std::string path;
			ss >> path;
			try {
				Texture text;
				text.loadTexture("resources/" + path);
				text.openGl2DTextureGen();
				current.setTexture(token, std::move(text));
			} catch (std::runtime_error) {
				std::cerr << "Warning texture not loaded" << path << std::endl;
			}
		}
	}
	if (!current.getName().empty())
		this->materials[current.getName()] = current;
};

void	ObjImporter::parseVertexLine(std::istringstream& iss) {
	Vector<float> vec(3);
	if (!(iss >> vec.x() >> vec.y() >> vec.z()))
		throw std::runtime_error("Error : Invalid v value");
	v.push_back(vec);
};

void	ObjImporter::parseTexCoordLine(std::istringstream& iss) {
	Vector<float> vec(3);
	if ((!(iss >> vec.x() >> vec.y())))
		throw std::runtime_error("Error : Invalid vt value");
	vt.push_back(vec);
}

void	ObjImporter::parseNormalLine(std::istringstream& iss) {
	Vector<float> vec(3);
	if ((!(iss >> vec.x() >> vec.y() >> vec.z())))
		throw std::runtime_error("Error : Invalid vn value");
	vn.push_back(vec);
}

void	ObjImporter::parseFaceLine(std::istringstream& iss) {
	Face face;
	face.materialName = currentMaterial;
	std::string vertStr;
	while (iss >> vertStr) {
		face.fvertices.push_back(parseFaceElement(vertStr));
	}
	f.push_back(face);
}

void	ObjImporter::parseMtlUseLine(std::istringstream& iss) {
	std::string name;
	iss >> name;
	this->currentMaterial = name;
}

void	ObjImporter::parseMtlLibLine(std::istringstream& iss) {
	std::string mtlFile;
	iss >> mtlFile;
	loadMtlFile(mtlFile);
}

void ObjImporter::parseObjFile(const std::string& fileName) {
		std::ifstream file;
		file.open(fileName);
		if (!file.is_open())
			throw std::runtime_error("Error: cannot open file");
		std::string line;
		while(std::getline(file, line))
		{
			if (line.empty())
				continue ; 
			std::istringstream iss(line);
			std::string token;
			iss >> token;
			if (token == "v") parseVertexLine(iss);
			else if (token == "f") parseFaceLine(iss);
			else if (token == "vt") parseTexCoordLine(iss);
			else if (token == "vn") parseNormalLine(iss);
			else if (token == "mtllib") parseMtlLibLine(iss);
			else if (token == "usemtl") parseMtlUseLine(iss);
		}
		file.close();
}

void ObjImporter::CenterAndNormalize() {
	colors = {
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.2f, 0.2f},
        {0.4f, 0.4f, 0.4f},
        {0.6f, 0.6f, 0.6f},
        {0.8f, 0.8f, 0.8f},
        {0.9f, 0.9f, 0.9f}
	};
	this->minVert = this->maxVert = v[0];
	for (const Vector<float>& vert : v) {
		if (vert.x() < this->minVert.x()) this->minVert.x() = vert.x();
		if (vert.y() < this->minVert.y()) this->minVert.y() = vert.y();
		if (vert.z() < this->minVert.z()) this->minVert.z() = vert.z();

		if (vert.x() > this->maxVert.x()) this->maxVert.x() = vert.x();
		if (vert.y() > this->maxVert.y()) this->maxVert.y() = vert.y();
		if (vert.z() > this->maxVert.z()) this->maxVert.z() = vert.z();
	}

	this->center = { 
		(this->minVert.x() + this->maxVert.x()) / 2.0f,
		(this->minVert.y() + this->maxVert.y()) / 2.0f,
		(this->minVert.z() + this->maxVert.z()) / 2.0f 
	};

	float radiusX = (this->maxVert.x() - this->minVert.x()) / 2.0f;
	float radiusY = (this->maxVert.y() - this->minVert.y()) / 2.0f;
	float radiusZ = (this->maxVert.z() - this->minVert.z()) / 2.0f;
	this->radius = std::max({radiusX, radiusY, radiusZ});

	this->scaleFactor = 1.0f / this->radius;
};

Vector<float> ObjImporter::computeFaceNormal(const Vector<float>& p0, const Vector<float>& p1, const Vector<float>& p2) const {
	Vector<float> edge1 = p1 - p0;
	Vector<float> edge2 = p2 - p0;
	return cross_product(edge1, edge2).normalize();
}

Vector<float> ObjImporter::getVertexNormal(const FaceVertex& fv) const {
	if (fv.vn != -1)
		return vn[fv.vn];
	return this->smoothNormals[fv.v];
};

Vector<float> ObjImporter::getVertexUV(const FaceVertex& fv, const Vector<float>& pos) const {
	if (fv.vt != -1)
		return vt[fv.vt];
	Vector <float> p = (pos - this->center) * this->scaleFactor;
	float u = 0.5f + atan2(pos.z(), pos.x()) / (2.0f * M_PI);
	float v = 0.5f - asin(pos.y() / this->radius) / M_PI;
	if (u < 0.0f) u += 1.0f;
	if (u > 1.0f) u -= 1.0f;
	return {u, v};
};

void ObjImporter::pushVertex(MaterialMesh& mesh, const Vector<float>& pos, const std::array<float, 3>& color, const Vector<float>& normal, const Vector<float>& uv) {
	mesh.vertices.push_back((pos.x() - this->center.x()) * this->scaleFactor);
	mesh.vertices.push_back((pos.y() - this->center.y()) * this->scaleFactor);
	mesh.vertices.push_back((pos.z() - this->center.z()) * this->scaleFactor);

	mesh.vertices.push_back(color[0]);
	mesh.vertices.push_back(color[1]);
	mesh.vertices.push_back(color[2]);

	mesh.vertices.push_back(normal.x());
	mesh.vertices.push_back(normal.y());
	mesh.vertices.push_back(normal.z());

	mesh.vertices.push_back(uv.x());
	mesh.vertices.push_back(uv.y());
};


void ObjImporter::BuildRenderMesh() {
	if (this->culling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	std::unordered_map<std::string, MaterialMesh> tempMesh;
	for (size_t faceIndex = 0; faceIndex < f.size(); ++faceIndex) {

		const auto& face = f[faceIndex];

		std::string matName = face.materialName.empty() ? "None" : face.materialName;
		if (tempMesh.find(matName) == tempMesh.end())
			tempMesh[matName].mat = (materials.count(matName) ? &materials[matName] : nullptr);
		MaterialMesh& mesh = tempMesh[matName];

		std::array<float, 3> color = colors[faceIndex % colors.size()];

		for (size_t j = 1; j + 1 < face.fvertices.size(); ++j) {
			FaceVertex fv0 = face.fvertices[0];
			FaceVertex fv1 = face.fvertices[j];
			FaceVertex fv2 = face.fvertices[j + 1];

			Vector<float> pos0 = v[fv0.v], pos1 = v[fv1.v], pos2 = v[fv2.v];

			Vector<float> n0 = getVertexNormal(fv0);
			Vector<float> n1 = getVertexNormal(fv1);
			Vector<float> n2 = getVertexNormal(fv2);
		
			Vector<float> uv0 = getVertexUV(fv0, pos0);
			Vector<float> uv1 = getVertexUV(fv1, pos1);
			Vector<float> uv2 = getVertexUV(fv2, pos2);

			pushVertex(mesh, pos0, color, n0, uv0);
			pushVertex(mesh, pos1, color, n1, uv1);
			pushVertex(mesh, pos2, color, n2, uv2);

		}
	}
	meshes.clear();
	for (auto& pair : tempMesh) {
		meshes.push_back(std::move(pair.second));
	}
};

static inline std::size_t hash_tuple(int a, int b, int c) {
    return std::hash<long long>()(((long long)a << 42) ^ ((long long)b << 21) ^ (long long)c);
}

void ObjImporter::normalsHandler()
{
    bool hasNormals = true;
    for (auto &face : this->f) {
        for (auto &fv : face.fvertices) {
            if (fv.vn == -1)
                hasNormals = false;
        }
    }
    if (hasNormals)
        return ;

    const float eps = 1e-6f;
    std::unordered_map<std::size_t, std::vector<int>> posGroups;
    posGroups.reserve(v.size()*2);

    for (size_t i = 0; i < v.size(); ++i) {
        int xi = static_cast<int>(std::floor(v[i].x() / eps));
        int yi = static_cast<int>(std::floor(v[i].y() / eps));
        int zi = static_cast<int>(std::floor(v[i].z() / eps));
        std::size_t h = hash_tuple(xi, yi, zi);
        posGroups[h].push_back((int)i);
    }

    this->smoothNormals.assign(v.size(), Vector<float>{0.0f, 0.0f, 0.0f});

    for (auto &face : f) {
        if (face.fvertices.size() < 3) continue;
        for (size_t j = 1; j + 1 < face.fvertices.size(); ++j) {
            FaceVertex fv0 = face.fvertices[0];
            FaceVertex fv1 = face.fvertices[j];
            FaceVertex fv2 = face.fvertices[j + 1];

            Vector<float> p0 = v[fv0.v], p1 = v[fv1.v], p2 = v[fv2.v];
            Vector<float> faceNormal = computeFaceNormal(p0, p1, p2);

            auto addToGroup = [&](int idx) {
                int xi = static_cast<int>(std::floor(v[idx].x() / eps));
                int yi = static_cast<int>(std::floor(v[idx].y() / eps));
                int zi = static_cast<int>(std::floor(v[idx].z() / eps));
                std::size_t h = hash_tuple(xi, yi, zi);
                auto &vec = posGroups[h];
                for (int k : vec)
                    smoothNormals[k] += faceNormal;
            };

            addToGroup(fv0.v);
            addToGroup(fv1.v);
            addToGroup(fv2.v);
        }
    }

    for (auto &n : smoothNormals) { 
        float len2 = n.x()*n.x() + n.y()*n.y() + n.z()*n.z();
        if (len2 > 1e-12f)
            n = n.normalize();
        else
            n = Vector<float>{0.0f, 1.0f, 0.0f};
    }
}

void ObjImporter::loadObj(const std::string& fileName, bool culling) {
	if (!culling)
		this->culling = false;
	if (fileName.size() > 4 || fileName.substr(fileName.size() - 4) == ".obj")
   		parseObjFile(fileName);
	else
		throw std::runtime_error("Error: wrong obj extension file");
    parseObjFile(fileName);
    normalsHandler();
    CenterAndNormalize();
    BuildRenderMesh();
}