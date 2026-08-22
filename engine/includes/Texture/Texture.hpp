#pragma once
#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <zlib.h>
#include <png.h>
#include <jpeglib.h>
#include <array>
#include <vector>
#include <cstring>

/** @brief Loads an image from disk (BMP, PNG, JPEG) and uploads it as an OpenGL 2D texture. */
class Texture {
	private:
		GLuint id;
		u_int32_t width;
		u_int32_t height;
		GLenum inputFormat;
		int bpp;                          // Bits per pixel.
		std::vector<unsigned char> data;

		/** @brief Decodes a BMP file into @p data. */
		void loadBMP(const std::string& path);
		/** @brief Decodes a PNG file into @p data using libpng. */
		void loadPNG(const std::string& path);
		/** @brief Decodes a JPEG file into @p data using libjpeg. */
		void loadJPEG(const std::string& path);
	public:
		Texture(): id(0), width(0), height(0) {};

		Texture(Texture&& other) noexcept
        : id(other.id), width(other.width), height(other.height),
          inputFormat(other.inputFormat), bpp(other.bpp), data(std::move(other.data))
		{
			other.id = 0;
		}

		Texture& operator=(Texture&& other) noexcept {
			if (this != &other) {
				if (id) glDeleteTextures(1, &id);

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

		/** @brief Detects the image format from the file extension and loads it. */
		void loadTexture(const std::string& path);
		/** @brief Creates and configures a GL_TEXTURE_2D object and uploads the decoded pixel data. */
		void openGl2DTextureGen();
		/**
		 * @brief Reconstructs scanline-ordered pixel data from a compressed/raw stream.
		 * @param textures     Output buffer for the final pixel data.
		 * @param decompressed Raw decompressed bytes from the image stream.
		 * @param bpp          Bytes per pixel.
		 */
		void scanlineInterpreter(std::vector<unsigned char>& textures, std::vector<unsigned char>& decompressed, size_t bpp);

		void bind() const { glBindTexture(GL_TEXTURE_2D, id); };
		void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); };
};
