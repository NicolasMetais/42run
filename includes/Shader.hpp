#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <glad/glad.h>

/** @brief Compiles, links, and exposes uniform setters for an OpenGL shader program. */
class Shader {
	private:
		GLuint id;
		// une location de uniform ne change jamais une fois le programme linke :
		// on evite un appel driver (glGetUniformLocation, recherche par string)
		// a chaque frame pour chaque uniform, sur chaque submesh, x2 passes.
		mutable std::unordered_map<std::string, GLint> uniformCache;
		/** @brief Renvoie la location d'un uniform, mise en cache apres la premiere requete. */
		GLint cachedLocation(const std::string& name) const;
		/** @brief Compiles a single shader stage from a source file path. */
		GLuint compileShader(GLenum type, const std::string& path);
		/** @brief Reads the full text of a shader source file. */
		std::string readShaderFile(const std::string& path);

	public:
		Shader() : id(0) {};
		/** @brief Compiles and links a vertex + fragment shader pair from file paths. */
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader() { glDeleteProgram(this->id); };

		void bind() const;
		void unbind() const;

		/** @brief Uploads a column-major 4x4 matrix uniform (transpose = true). */
		void setMatrix4_true(const std::string& name, const float* value) const;
		/** @brief Uploads a column-major 4x4 matrix uniform (transpose = false). */
		void setMatrix4_false(const std::string& name, const float* value) const;
		/** @brief Uploads a vec2 uniform. */
		void setVec2(const std::string& name, const float x, const float y) const;
		/** @brief Uploads a vec3 uniform. */
		void setVec3(const std::string& name, const float x, const float y, const float z) const;
		/** @brief Uploads a vec4 uniform. */
		void setVec4(const std::string& name, const float x, const float y, const float z, const float w) const;
		/** @brief Uploads an int uniform (also used for sampler slots). */
		void setInt(const std::string& name, const int value) const;
		/** @brief Uploads a float uniform. */
		void setFloat(const std::string& name, const float value) const;
		/** @brief Returns the location of a uniform variable, or -1 if not found. */
		GLint getUniformLocation(const std::string& name) const;
		GLuint getId() const { return this->id; };
};
