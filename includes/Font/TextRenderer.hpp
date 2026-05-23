#pragma once
#include <iostream>
#include <vector>
#include <TextureManager.hpp>
#include <Font/AtlasFont.hpp>
#include <Shader.hpp>
#include <glad/glad.h>

class TextRenderer {
    private:
        TextureManager& textureManager;
        Shader msdf;
        GLuint VAO;
	    GLuint VBO;
        int screenW;
        int screenH;
    public:
        TextRenderer(TextureManager& TextureManager, int screenW, int screenH);
        void drawText(const std::string& text, std::string name, float x, float y, float size, const AtlasFont& font);

};