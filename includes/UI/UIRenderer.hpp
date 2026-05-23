#pragma once
#include <iostream>
#include <Texture/TextureManager.hpp>
#include <Shader.hpp>
#include <glad/glad.h>

class UIRenderer {
    private:
        TextureManager& textureManager;
        Shader menu;
        Shader water;
        GLuint VAO;
        GLuint VBO;
        int screenW;
        int screenH;
    public:
        UIRenderer(TextureManager& TextureManager, int w, int h);
        void drawUIComponent(Texture& tex, float x, float y, float z, float width, float height);
        void drawUIComponent(float x, float y, float z, float width, float height, float r, float g, float b);
        void drawWaterBackground(float mouseX, float mouseY, float time);

};