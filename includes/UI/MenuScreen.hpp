#pragma once
#include <stack>
#include <Input/Keyboard.hpp>
#include <UI/UIRenderer.hpp>
#include <Font/TextRenderer.hpp>
#include <Font/FontManager.hpp>


struct RenderContext {
    TextRenderer& textRenderer;
    FontManager& fontManager;
    UIRenderer& uiRenderer;
    int screenW;
    int screenH;
    RenderContext(TextRenderer& textRenderer, FontManager& fontManager, UIRenderer& uiRenderer, int screenW, int screenH) :
        textRenderer(textRenderer), fontManager(fontManager), uiRenderer(uiRenderer), screenW(screenW), screenH(screenH) {};
};

class MenuScreen {
    public:
        virtual void update(Keyboard& kb, std::stack<MenuScreen*>& stack) = 0;
        virtual void draw(RenderContext& tools) = 0;
        virtual ~MenuScreen() = default;
};