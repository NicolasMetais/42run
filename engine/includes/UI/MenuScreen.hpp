#pragma once
#include <stack>
#include <vector>
#include <memory>
#include <Input/Keyboard.hpp>
#include <Input/Mouse.hpp>
#include <UI/UIRenderer.hpp>
#include <Font/TextRenderer.hpp>
#include <Font/FontManager.hpp>
#include <Texture/TextureManager.hpp>

/** @brief Renvoie l'index de la ligne survolee par la souris parmi des items empiles
 * verticalement a rowY[i], ou -1 si aucun. Meme rectangle de detection pour tous. */
inline int hitTestMenuRow(Mouse& mouse, const std::vector<float>& rowY, float left, float right, float rowHeight) {
    for (size_t i = 0; i < rowY.size(); ++i)
        if (mouse.getX() >= left && mouse.getX() <= right
            && mouse.getY() >= rowY[i] - rowHeight * 0.5f && mouse.getY() <= rowY[i] + rowHeight * 0.5f)
            return (int)i;
    return -1;
}

/** @brief drawText positionne par la ligne de base (le corps des lettres remonte
 * au-dessus de "y"). Renvoie le decalage vers le haut pour retrouver le centre
 * visuel du texte, lu depuis les vraies metriques de la police (pas de nombre en dur). */
inline float textBaselineToVisualCenter(const AtlasFont& font, float textSize) {
    return font.ascender * textSize * 0.5f;
}


struct RenderContext {
    TextRenderer& textRenderer;
    FontManager& fontManager;
    UIRenderer& uiRenderer;
    TextureManager& textureManager; // pour resoudre une image brute (icones UI) en Texture&
    int screenW;
    int screenH;
    RenderContext(TextRenderer& textRenderer, FontManager& fontManager, UIRenderer& uiRenderer, TextureManager& textureManager, int screenW, int screenH) :
        textRenderer(textRenderer), fontManager(fontManager), uiRenderer(uiRenderer), textureManager(textureManager), screenW(screenW), screenH(screenH) {};
};

class MenuScreen {
    public:
        virtual void update(Keyboard& kb, Mouse& mouse, FontManager& fontManager, int screenW, int screenH, std::stack<std::unique_ptr<MenuScreen>>& stack) = 0;
        virtual void draw(RenderContext& tools) = 0;
        virtual ~MenuScreen() = default;
};
