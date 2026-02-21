#pragma once
#include <iostream>
#include <fstream>
#include <include/glad/glad.h>
#include <zlib.h>
#include <png.h>
#include <jpeglib.h>
#include <array>
#include <vector>
#include <cstring>

class Texture {
	private:
		GLuint id;
		u_int32_t width;
		u_int32_t height;
		GLenum inputFormat;
		int bpp;
		std::vector<unsigned char> data;
		void loadBMP(const std::string& path);
		void loadPNG(const std::string& path);
		void loadJPEG(const std::string& path);
	public:
		Texture(): id(0), width(0), height(0) {};
		    Texture(Texture&& other) noexcept
        : id(other.id), width(other.width), height(other.height),
          inputFormat(other.inputFormat), bpp(other.bpp), data(std::move(other.data))
		{
			other.id = 0; // éviter double suppression
		}

		Texture& operator=(Texture&& other) noexcept {
			if (this != &other) {
				if (id) glDeleteTextures(1, &id); // on supprime l'ancienne texture

				id = other.id;
				width = other.width;
				height = other.height;
				inputFormat = other.inputFormat;
				bpp = other.bpp;
				data = std::move(other.data);

				other.id = 0;
			}
			return *this;
		}
		~Texture() { if (id) glDeleteTextures(1, &id); };
		GLuint getId() const { return this->id; };
		u_int32_t getwidth() const { return this->width; };
		u_int32_t getHeight() const { return this->height; };
		GLenum getinputFormat() const { return this->inputFormat; };
		int getbpp() const { return this->bpp; };
		std::vector<unsigned char> getData() const { return this->data; };
		void loadTexture(const std::string& path);
		void openGl2DTextureGen();
		void scanlineInterpreter(std::vector<unsigned char>& textures, std::vector<unsigned char>& decompressed, size_t bpp);
		void bind() const { glBindTexture(GL_TEXTURE_2D, id); };
		void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); };
};