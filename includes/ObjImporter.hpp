#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Matrix/Matrix.hpp"
#include <array>
#include <include/glad/glad.h>
#include <Mat.hpp>
#include <Texture.hpp>
#include <MeshData.hpp>
#include <unordered_map>

// struct MaterialMesh {
// 	Material *mat;
// 	GLuint VAO, VBO;
// 	size_t vertexCount;
// 	std::vector<float> vertices;
// };

class ObjImporter {
	private:
		std::string name;
		std::vector<Vector<float>> v;
		std::vector<Vector<float>> vt;
		std::vector<Vector<float>> vn;
		std::vector<Vector<float>> smoothNormals;
		Vector<float> minVert;
		Vector<float> maxVert;
		std::string currentMaterial;
		struct FaceVertex {
			int v = -1;
			int vt = -1;
			int vn = -1;
		};
		struct Face {
			std::vector<FaceVertex> fvertices;
			std::string materialName;
		};
		std::unordered_map <std::string, Mat> materials;
		std::vector<Face> f;
	public:
		ObjImporter () = default;

		void parseObjFile(const std::string& fileName);
		void parseVertexLine(std::istringstream& iss);
		void parseTexCoordLine(std::istringstream& iss);
		void parseNormalLine(std::istringstream& iss);
		void parseFaceLine(std::istringstream& iss);
		void parseMtlUseLine(std::istringstream& iss);
		void parseMtlLibLine(std::istringstream& iss);
		FaceVertex parseFaceElement(const std::string& part);

		void loadMtlFile(const std::string& mtlname);

		void normalsHandler();

		MeshData buildMeshData();

		Vector<float> computeFaceNormal(const Vector<float>& p0, const Vector<float>& p1, const Vector<float>& p2) const;
		Vector<float> getVertexNormal(const FaceVertex& fv) const;
		Vector<float> getVertexUV(const FaceVertex& fv, const Vector<float>& pos) const;

		MeshData loadObj(const std::string& fileName);

		const std::vector<Vector<float>>& getVertices() const { return v; };
		const Vector<float>& getMinVertices() const { return minVert; };
		const Vector<float>& getMaxVertices() const { return maxVert; };

		void setName(const std::string& name) { this->name = name; };
};