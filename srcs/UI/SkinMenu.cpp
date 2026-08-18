#include <UI/SkinMenu.hpp>

void SkinMenu::update(Keyboard& kb, Mouse& mouse, FontManager& fontManager, int screenW, int screenH, std::stack<MenuScreen*>& stack) {
        if (kb.consumeMenuUp() && selector > 0)
            selector--;
        if (kb.consumeMenuDown() && selector < 3)
            selector++;

        float offset = textBaselineToVisualCenter(fontManager.getFont("CalliCat"), screenH * 0.04f);
        std::vector<float> positions = { screenH * 0.5f - offset, screenH * 0.6f - offset, screenH * 0.7f - offset, screenH * 0.8f - offset };
        int hovered = hitTestMenuRow(mouse, positions, screenW * 0.38f, screenW * 0.75f, screenH * 0.07f);
        if (mouse.consumeMoved() && hovered >= 0) selector = hovered;
        bool clicked = mouse.consumeClick();

        if (kb.consumeMenuConfirm() || (clicked && hovered >= 0)) {
            switch (selector) {
                case 0:
                    std::cout << "WIP" << std::endl; break;
                case 1:
                    std::cout << "WIP" << std::endl; break;
                case 2:
                    std::cout << "WIP" << std::endl; break;
                case 3:
                    stack.pop(); break ;
                default: break;

            }
        }
};

void SkinMenu::draw(RenderContext& tools) {
    float positions[] = { tools.screenH * 0.5f, tools.screenH * 0.6f, tools.screenH * 0.7f, tools.screenH * 0.8f };
    tools.textRenderer.drawText("Select your skin :", "CalliCat", tools.screenW * 0.38f, tools.screenH * 0.4f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    Texture& pawTex = tools.textureManager.getOrLoad("cat_paw.png");
    float pawSize = tools.screenH * 0.04f;
    float baselineToVisualCenter = textBaselineToVisualCenter(tools.fontManager.getFont("CalliCat"), tools.screenH * 0.04f);
    tools.uiRenderer.drawUIComponent(pawTex, tools.screenW * 0.40f, positions[selector] - baselineToVisualCenter - pawSize * 0.5f, 0.0f, pawSize, pawSize);
    tools.textRenderer.drawText("Skin1", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.5f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Skin2", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.6f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Skin3", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.7f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Back", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.8f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
};