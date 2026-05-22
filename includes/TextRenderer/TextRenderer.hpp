#pragma once
#include <iostream>
#include <TextureManager.hpp>
#include <TextureManager.hpp>
#include <Shader.hpp>
#include <glad/glad.h>

class TextRenderer {
    private:
        TextureManager& textureManager;
        Shader msdf;
        GLuint VAO;
	    GLuint VBO;
    public:
        TextRenderer(TextureManager& TextureManager);
        void drawText();

};