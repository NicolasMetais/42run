#pragma once
#include <iostream>
#include <TextureManager.hpp>
#include <Shader.hpp>
#include <glad/glad.h>

class UIRenderer {
    private:
        TextureManager& textureManager;
        Shader menu;
        GLuint VAO;
        GLuint VBO;
    public:
        UIRenderer(TextureManager& TextureManager);
        void drawUIComponent(Texture& tex, float x, float y, float z, float width, float height);
        void drawUIComponent(float x, float y, float z, float width, float height);

};