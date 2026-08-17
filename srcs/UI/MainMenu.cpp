#include <UI/MainMenu.hpp>

MainMenu::MainMenu(std::function<void()> onPlay, std::function<void()> onQuit) :onPlay(onPlay), onQuit(onQuit) {};

void MainMenu::update(Keyboard& kb, std::stack<MenuScreen*>& stack) {
        if (kb.consumeMenuUp() && selector > 0)
            selector--;
        if (kb.consumeMenuDown() && selector < 2)
            selector++;
        if (kb.consumeMenuConfirm()) {
            switch (selector) {
                case 0:
                    onPlay(); break;
                case 1:
                    stack.push(new SkinMenu()); break ;
                case 2:
                    onQuit(); break;
                default: break;

            }
        }
};

void MainMenu::draw(RenderContext& tools) {
    float positions[] = { tools.screenH * 0.4f, tools.screenH * 0.5f, tools.screenH * 0.6f, tools.screenH * 0.7f };
    tools.textRenderer.drawText(">", "CalliCat", tools.screenW * 0.40f, positions[selector], tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Play", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.4f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Skins", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.5f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Quit", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.6f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
};