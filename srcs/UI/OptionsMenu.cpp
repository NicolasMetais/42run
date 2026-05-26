#include <UI/OptionsMenu.hpp>
#include <UI/MainMenu.hpp>

void OptionsMenu::update(Keyboard& kb, std::stack<MenuScreen*>& stack) {
        if (kb.consumeMenuUp() && selector > 0)
            selector--;
        if (kb.consumeMenuDown() && selector < 1)
            selector++;
        if (kb.consumeMenuConfirm()) {
            switch (selector) {
                case 0:
                    std::cout << "WIP" << std::endl; break;
                case 1:
                    stack.pop(); break ;
                default: break;

            }
        }
};

void OptionsMenu::draw(RenderContext& tools) {
    float positions[] = { tools.screenH * 0.4f, tools.screenH * 0.5f };
    tools.textRenderer.drawText(">", "Roboto", tools.screenW * 0.40f, positions[selector], tools.screenH * 0.04f, tools.fontManager.getFont("Roboto"));
    tools.textRenderer.drawText("placeholder option", "Roboto", tools.screenW * 0.47f, tools.screenH * 0.4f, tools.screenH * 0.04f, tools.fontManager.getFont("Roboto"));
    tools.textRenderer.drawText("Back", "Roboto", tools.screenW * 0.47f, tools.screenH * 0.5f, tools.screenH * 0.04f, tools.fontManager.getFont("Roboto"));
};