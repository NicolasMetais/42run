#include <Font/TextRenderer.hpp>

TextRenderer::TextRenderer(TextureManager& TextureManager, int w, int h) : textureManager(TextureManager), msdf("srcs/Font/msdf.vs", "srcs/Font/msdf.fs"), screenW(w), screenH(h) {
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

};

//C HORRIBLE FO REFACTO MAIS OMG J'AI LA BIG FLEMME LA
void TextRenderer::drawText(const std::string& text, std::string name, float x, float y, float size, const AtlasFont& font) {
    Texture& FontTex = textureManager.getOrLoad(name + ".png");
    glActiveTexture(GL_TEXTURE0);
    msdf.bind();
    float proj[16] = {
        2.0f/screenW, 0,             0, 0,
        0,           -2.0f/screenH,  0, 0,
        0,            0,            -1, 0,
       -1,            1,             0, 1
    };
    glUniformMatrix4fv(msdf.getUniformLocation("projection"), 1, GL_FALSE, proj);
    glUniform3f(msdf.getUniformLocation("textColor"), 1.0f, 1.0f, 1.0f);
    glUniform1i(msdf.getUniformLocation("fontTex"), 0);
    FontTex.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, text.size() * 6 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW); //dynamyc init for text construction
    std::vector<float> vertices;
    vertices.reserve(text.size() * 24);
    for (auto& charac : text) {
        if(!font.glyphs.count(charac)) {
            if (font.glyphs.count(32))
                x += font.glyphs.at(32).advance * size;
            else
                x += 0.25f * size;
            continue;
        }
        GlyphData glyph = font.glyphs.at(charac);

        float planeLeft = x + glyph.planeLeft * size;
        float planeRight = x + glyph.planeRight * size;
        
        float planeTop = y - glyph.planeTop * size;
        float planeBottom = y - glyph.planeBottom * size;

        float atlasLeft = glyph.atlasLeft / font.width;
        float atlasRight = glyph.atlasRight / font.width;

        float atlasTop = (font.height - glyph.atlasTop) / font.height;
        float atlasBottom = (font.height - glyph.atlasBottom) / font.height;

        vertices.push_back(planeLeft);
        vertices.push_back(planeTop);

        vertices.push_back(atlasLeft);
        vertices.push_back(atlasTop);

        vertices.push_back(planeRight);
        vertices.push_back(planeTop);

        vertices.push_back(atlasRight);
        vertices.push_back(atlasTop);

        vertices.push_back(planeLeft);
        vertices.push_back(planeBottom);

        vertices.push_back(atlasLeft);
        vertices.push_back(atlasBottom);

        vertices.push_back(planeRight);
        vertices.push_back(planeTop);

        vertices.push_back(atlasRight);
        vertices.push_back(atlasTop);

        vertices.push_back(planeRight);
        vertices.push_back(planeBottom);

        vertices.push_back(atlasRight);
        vertices.push_back(atlasBottom);

        vertices.push_back(planeLeft);
        vertices.push_back(planeBottom);

        vertices.push_back(atlasLeft);
        vertices.push_back(atlasBottom);

        x += glyph.advance * size; //movemont on the cursor to put the next letter to the right
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 4);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
};


