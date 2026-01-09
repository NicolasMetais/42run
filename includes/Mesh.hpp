#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Matrix/Matrix.hpp"
#include <array>
#include <include/glad/glad.h>
#include <Material.hpp>
#include <Texture.hpp>
#include <unordered_map>

struct MaterialMesh {
	Material *mat;
	GLuint VAO, VBO;
	size_t vertexCount;
	std::vector<float> vertices;
};

class Mesh {
	private:
		std::string name;
		std::vector<Vector<float>> v;
		std::vector<Vector<float>> vt;
		std::vector<Vector<float>> vn;
		std::vector<float> centeredVertices;
		std::vector<Vector<float>> smoothNormals;
		std::vector<MaterialMesh> meshes;
		Vector<float> minVert;
		Vector<float> maxVert;
		Vector<float> center;
		std::vector<std::array<float, 3>> colors;
		float radius;
		float scaleFactor;
		std::string currentMaterial;
		struct FaceVertex {
			int v;
			int vt;
			int vn;
		};
		struct Face {
			std::vector<FaceVertex> fvertices;
			std::string materialName;
		};
		std::unordered_map <std::string, Material> materials;
		std::vector<Face> f;
		bool culling = true;
	public:
		Mesh () {};
		void loadObj(const std::string& fileName, bool culling);
		void loadMtlFile(const std::string& mtlname);
		FaceVertex parseFaceElement(const std::string& part);
		std::vector<MaterialMesh>& getMeshes() { return meshes; };
		const std::vector<Vector<float>>& getVertices() const { return v; };
		const std::vector<float>& getCenteredVertices() const { return centeredVertices; };
		const Vector<float>& getMinVertices() const { return minVert; };
		const Vector<float>& getMaxVertices() const { return maxVert; };
		const float& getRadius() const { return radius; };
		void parseObjFile(const std::string& fileName);
		void CenterAndNormalize();
		void BuildRenderMesh();
		void normalsHandler();
		void parseVertexLine(std::istringstream& iss);
		void parseTexCoordLine(std::istringstream& iss);
		void parseNormalLine(std::istringstream& iss);
		void parseFaceLine(std::istringstream& iss);
		void parseMtlUseLine(std::istringstream& iss);
		void parseMtlLibLine(std::istringstream& iss);
		void setName(const std::string& name) { this->name = name; };
		Vector<float> computeFaceNormal(const Vector<float>& p0, const Vector<float>& p1, const Vector<float>& p2) const;
		Vector<float> getVertexNormal(const FaceVertex& fv) const;
		Vector<float> getVertexUV(const FaceVertex& fv, const Vector<float>& pos) const;
		void pushVertex(MaterialMesh& mesh, const Vector<float>& pos, const std::array<float, 3>& color, const Vector<float>& normal, const Vector<float>& uv);
		
};