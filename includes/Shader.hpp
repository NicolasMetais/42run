#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <include/glad/glad.h>

class Shader {
	private:
		GLuint id;
		GLuint compileShader(GLenum type, const std::string& path);
		std::string readShaderFile(const std::string& path);
	
	public:
		Shader() : id(0) {};
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;
		~Shader() { glDeleteProgram(this->id); };

		void bind() const; //PROTEGER POUR NE APS LANCER UN SHADER INVALIDE ET EVITER SEGFAULT
		void unbind() const;

		void setMatrix4(const std::string& name, const float* value) const;
		void setVec3(const std::string& name, const float x, const float y, const float z) const;
		void setInt(const std::string& name, const int value) const;
		void setfloat(const std::string& name, const float value) const;
		GLint getUniformLocation(const std::string& name) const;
		GLuint getId() const { return this->id; };



};