#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Matrix/Matrix.hpp"
#include <array>
#include <glad/glad.h>
#include <Mat.hpp>
#include <Texture/Texture.hpp>
#include <MeshData.hpp>
#include <unordered_map>

/** @brief Parses Wavefront OBJ files (and their MTL libraries) into a MeshData. */
class ObjImporter {
	private:
		std::string name;
		std::vector<Vector<float>> v;           ///< Raw vertex positions.
		std::vector<Vector<float>> vt;          ///< Raw texture coordinates.
		std::vector<Vector<float>> vn;          ///< Raw vertex normals.
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
		std::unordered_map<std::string, Mat> materials;
		std::vector<Face> f;
	public:
		ObjImporter() = default;

		/** @brief Parses the entire OBJ file, dispatching each line to the appropriate sub-parser. */
		void parseObjFile(const std::string& fileName);
		/** @brief Parses a "v" line and appends the position to the vertex list. */
		void parseVertexLine(std::istringstream& iss);
		/** @brief Parses a "vt" line and appends the UV coordinate. */
		void parseTexCoordLine(std::istringstream& iss);
		/** @brief Parses a "vn" line and appends the normal. */
		void parseNormalLine(std::istringstream& iss);
		/** @brief Parses an "f" line and appends the face with its per-vertex indices. */
		void parseFaceLine(std::istringstream& iss);
		/** @brief Parses a "usemtl" line and sets the active material name. */
		void parseMtlUseLine(std::istringstream& iss);
		/** @brief Parses an "mtllib" line and triggers loading of the referenced MTL file. */
		void parseMtlLibLine(std::istringstream& iss);
		/** @brief Parses a single face element token ("v/vt/vn") into a FaceVertex. */
		FaceVertex parseFaceElement(const std::string& part);

		/** @brief Loads and parses a MTL material library file. */
		void loadMtlFile(const std::string& mtlname);

		/** @brief Computes smooth per-vertex normals by averaging adjacent face normals. */
		void normalsHandler();

		/** @brief Assembles all parsed data into a MeshData with GPU-ready submeshes. */
		MeshData buildMeshData();

		/** @brief Computes the face normal from three vertex positions. */
		Vector<float> computeFaceNormal(const Vector<float>& p0, const Vector<float>& p1, const Vector<float>& p2) const;
		/** @brief Returns the normal for a face vertex (from vn or computed). */
		Vector<float> getVertexNormal(const FaceVertex& fv) const;
		/** @brief Returns the UV coordinate for a face vertex (from vt or generated). */
		Vector<float> getVertexUV(const FaceVertex& fv, const Vector<float>& pos) const;

		/** @brief Convenience method: parses the OBJ file and returns the resulting MeshData. */
		MeshData loadObj(const std::string& fileName);

		const std::vector<Vector<float>>& getVertices() const { return v; };
		const Vector<float>& getMinVertices() const { return minVert; };
		const Vector<float>& getMaxVertices() const { return maxVert; };

		void setName(const std::string& name) { this->name = name; };
};
