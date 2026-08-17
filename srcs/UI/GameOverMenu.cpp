#include <UI/GameOverMenu.hpp>

GameOverMenu::GameOverMenu(std::function<void()> resetGame, std::function<void()> onMainMenu) : resetGame(resetGame), onMainMenu(onMainMenu) {};

void GameOverMenu::update(Keyboard& kb, std::stack<MenuScreen*>& stack) {
        (void)stack;
        if (kb.consumeMenuUp() && selector > 0)
            selector--;
        if (kb.consumeMenuDown() && selector < 0)
            selector++;
        if (kb.consumeMenuConfirm()) {
            switch (selector) {
                case 0:
                    resetGame();
                    onMainMenu();
                default: break;

            }
        }
};

void GameOverMenu::draw(RenderContext& tools) {
    tools.textRenderer.drawText("YOU LOST", "CalliCat", tools.screenW * 0.40f, tools.screenH * 0.2f, tools.screenH * 0.08f, tools.fontManager.getFont("CalliCat"));
    float positions[] = { tools.screenH * 0.5f};
    tools.textRenderer.drawText(">", "CalliCat", tools.screenW * 0.40f, positions[selector], tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Go away", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.5f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
};