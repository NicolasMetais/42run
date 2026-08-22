#pragma once
#include <iostream>
#include <vector>
#include <Texture/TextureManager.hpp>
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
        std::vector<float> vertices; // buffer reutilise entre les appels (evite une reallocation par drawText)
    public:
        TextRenderer(TextureManager& TextureManager, int screenW, int screenH);
        /** @brief Resynchronise les dimensions ecran utilisees pour la projection texte (a appeler au resize). */
        void updateScreenSize(int w, int h) { screenW = w; screenH = h; }
        float drawText(const std::string& text, std::string name, float x, float y, float size, const AtlasFont& font, float outlineWidth = 0.0f);

};