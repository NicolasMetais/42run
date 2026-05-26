#pragma once
#include <iostream>
#include <vector>
#include <Texture/TextureManager.hpp>
#include <Shader.hpp>
#include <glad/glad.h>

struct RippleDrop {
    float u;
    float v;
    float age = 0.0f;
};

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
        void drawSliderComponent(float x, float y, float z, float value, float width, float height, float r, float g, float b);
        void drawCheckboxComponent(float x, float y, float z, float width, float height, float r, float g, float b);
        void drawWaterBackground(Texture& tex, float time, const std::vector<RippleDrop>& drops);
};