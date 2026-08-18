#include <UI/UIRenderer.hpp>

UIRenderer::UIRenderer(TextureManager& TextureManager, int w, int h) : textureManager(TextureManager), menu("srcs/UI/menu.vs", "srcs/UI/menu.fs"), water("srcs/UI/water.vs", "srcs/UI/water.fs"), screenW(w), screenH(h) {
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
};

void UIRenderer::drawUIComponent(Texture& tex, float x, float y, float z, float width, float height) {
    (void)z; //FAUT CODER Z MAIS FLEMME POUR LE MOMENT VOILA
    glActiveTexture(GL_TEXTURE0);
    menu.bind();
    float proj[16] = {
        2.0f/screenW, 0,             0, 0,
        0,           -2.0f/screenH,  0, 0,
        0,            0,            -1, 0,
       -1,            1,             0, 1
    };
    menu.setMatrix4_false("projection", proj);
    menu.setInt("fontTex", 0);
    menu.setInt("useTexture", 1);
    tex.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // reduction propre pour les textures sources bien plus grandes que leur taille affichee
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    std::vector<float> vertices;

    float topLeft = x;
    float topRight = x + width;

    float bottomLeft = y;
    float bottomright = y + height;

    vertices.push_back(topLeft);            // (left, top)    ----    (right, top)
    vertices.push_back(bottomLeft);         //     |                       |
    vertices.push_back(0);                  //     |                       |
    vertices.push_back(0);                  // (left, bottom) ----  (right, bottom)

    vertices.push_back(topRight);
    vertices.push_back(bottomLeft);
    vertices.push_back(1);
    vertices.push_back(0);

    vertices.push_back(topLeft);
    vertices.push_back(bottomright);
    vertices.push_back(0);
    vertices.push_back(1);

    vertices.push_back(topRight);
    vertices.push_back(bottomLeft);
    vertices.push_back(1);
    vertices.push_back(0);

    vertices.push_back(topRight);
    vertices.push_back(bottomright);
    vertices.push_back(1);
    vertices.push_back(1);

    vertices.push_back(topLeft);
    vertices.push_back(bottomright);
    vertices.push_back(0);
    vertices.push_back(1);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 4);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
};

void UIRenderer::drawUIComponent(float x, float y, float z, float width, float height, float r, float g, float b, float a) {
    (void)z;
    glActiveTexture(GL_TEXTURE0);
    menu.bind();
    float proj[16] = {
        2.0f/screenW, 0,             0, 0,
        0,           -2.0f/screenH,  0, 0,
        0,            0,            -1, 0,
       -1,            1,             0, 1
    };
    menu.setMatrix4_false("projection", proj);
    menu.setInt("bottomTex", 0);
    menu.setVec4("color", r, g ,b, a);
    menu.setInt("useTexture", 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    std::vector<float> vertices;

    float topLeft = x;
    float topRight = x + width;

    float bottomLeft = y;
    float bottomright = y + height;

    vertices.push_back(topLeft);            // (left, top)    ----    (right, top)
    vertices.push_back(bottomLeft);         //     |                       |
    vertices.push_back(0);                  //     |                       |
    vertices.push_back(0);                  // (left, bottom) ----  (right, bottom)

    vertices.push_back(topRight);
    vertices.push_back(bottomLeft);
    vertices.push_back(1);
    vertices.push_back(0);

    vertices.push_back(topLeft);
    vertices.push_back(bottomright);
    vertices.push_back(0);
    vertices.push_back(1);

    vertices.push_back(topRight);
    vertices.push_back(bottomLeft);
    vertices.push_back(1);
    vertices.push_back(0);

    vertices.push_back(topRight);
    vertices.push_back(bottomright);
    vertices.push_back(1);
    vertices.push_back(1);

    vertices.push_back(topLeft);
    vertices.push_back(bottomright);
    vertices.push_back(0);
    vertices.push_back(1);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 4);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
};

void UIRenderer::drawWaterBackground(Texture& tex, float time, const std::vector<RippleDrop>& drops) {
    glActiveTexture(GL_TEXTURE0);
    water.bind();
    float proj[16] = {
        2.0f/screenW, 0,             0, 0,
        0,           -2.0f/screenH,  0, 0,
        0,            0,            -1, 0,
       -1,            1,             0, 1
    };
    water.setMatrix4_false("projection", proj);
    water.setFloat("time", time);
    water.setInt("bottomTex", 0);
    int count = std::min((int)drops.size(), 8);
    float dropData[24] = {};
    for (int i = 0; i < count; i++) {
        dropData[i*3]   = drops[i].u;
        dropData[i*3+1] = drops[i].v;
        dropData[i*3+2] = drops[i].age;
    }
    glUniform3fv(water.getUniformLocation("drops"), count, dropData);
    glUniform1i(water.getUniformLocation("dropCount"), count);
    tex.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    std::vector<float> vertices;

    float topLeft = 0;
    float topRight = this->screenW;

    float bottomLeft = 0;
    float bottomright = this->screenH;

    vertices.push_back(topLeft);            // (left, top)    ----    (right, top)
    vertices.push_back(bottomLeft);         //     |                       |
    vertices.push_back(0);                  //     |                       |
    vertices.push_back(0);                  // (left, bottom) ----  (right, bottom)

    vertices.push_back(topRight);
    vertices.push_back(bottomLeft);
    vertices.push_back(1);
    vertices.push_back(0);

    vertices.push_back(topLeft);
    vertices.push_back(bottomright);
    vertices.push_back(0);
    vertices.push_back(1);

    vertices.push_back(topRight);
    vertices.push_back(bottomLeft);
    vertices.push_back(1);
    vertices.push_back(0);

    vertices.push_back(topRight);
    vertices.push_back(bottomright);
    vertices.push_back(1);
    vertices.push_back(1);

    vertices.push_back(topLeft);
    vertices.push_back(bottomright);
    vertices.push_back(0);
    vertices.push_back(1);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 4);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
};

void UIRenderer::drawSliderComponent(float x, float y, float z, float value, float width, float height, float r, float g, float b) {
    drawUIComponent(x, y, z, width, height, 0.3f, 0.3f, 0.3f);
    drawUIComponent(x, y, z, width * value, height, r, g, b);
    drawUIComponent(x + width *  value - height * 0.5f, y, z, height, height, 1,1,1);
};

void UIRenderer::drawCheckboxComponent(float x, float y, float z, float width, float height, float r, float g, float b) {
    drawUIComponent(x, y, z, width, height, r, g, b);
};
