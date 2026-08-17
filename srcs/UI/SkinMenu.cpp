#include <UI/SkinMenu.hpp>

void SkinMenu::update(Keyboard& kb, std::stack<MenuScreen*>& stack) {
        if (kb.consumeMenuUp() && selector > 0)
            selector--;
        if (kb.consumeMenuDown() && selector < 3)
            selector++;
        if (kb.consumeMenuConfirm()) {
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
    tools.textRenderer.drawText(">", "CalliCat", tools.screenW * 0.40f, positions[selector], tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Skin1", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.5f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Skin2", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.6f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Skin3", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.7f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Back", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.8f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
};