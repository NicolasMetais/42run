#include <TextRenderer/TextRenderer.hpp>

TextRenderer::TextRenderer(TextureManager& TextureManager) : textureManager(textureManager), msdf("srcs/shader/msdf.vs", "srcs/shader/msdf.fs") {
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    glGenBuffers(1, nullptr); //empty init for dynamic draw text
    glBindBuffer(GL_DYNAMIC_DRAW, this->VBO); //dynamic draw to construct the words dynamically
};

void TextRenderer::drawText() {
    
};


