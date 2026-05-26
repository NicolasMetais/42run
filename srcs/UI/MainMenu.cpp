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
                    stack.push(new OptionsMenu()); break ;
                case 2:
                    onQuit(); break;
                default: break;

            }
        }
};

void MainMenu::draw(RenderContext& tools) {
    float positions[] = { tools.screenH * 0.4f, tools.screenH * 0.5f, tools.screenH * 0.6f };
    tools.textRenderer.drawText(">", "Roboto", tools.screenW * 0.40f, positions[selector], tools.screenH * 0.04f, tools.fontManager.getFont("Roboto"));
    tools.textRenderer.drawText("Play", "Roboto", tools.screenW * 0.47f, tools.screenH * 0.4f, tools.screenH * 0.04f, tools.fontManager.getFont("Roboto"));
    tools.textRenderer.drawText("Options", "Roboto", tools.screenW * 0.47f, tools.screenH * 0.5f, tools.screenH * 0.04f, tools.fontManager.getFont("Roboto"));
    tools.textRenderer.drawText("Quit", "Roboto", tools.screenW * 0.47f, tools.screenH * 0.6f, tools.screenH * 0.04f, tools.fontManager.getFont("Roboto"));
};