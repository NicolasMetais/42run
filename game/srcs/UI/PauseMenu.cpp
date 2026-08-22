#include <UI/PauseMenu.hpp>

PauseMenu::PauseMenu(std::function<void()> onResume, std::function<void()> resetGame, std::function<void()> onMainMenu) : onResume(onResume), resetGame(resetGame), onMainMenu(onMainMenu) {};

void PauseMenu::update(Keyboard& kb, Mouse& mouse, FontManager& fontManager, int screenW, int screenH, std::stack<std::unique_ptr<MenuScreen>>& stack) {
        if (kb.consumeMenuUp() && selector > 0)
            selector--;
        if (kb.consumeMenuDown() && selector < 2)
            selector++;

        float offset = textBaselineToVisualCenter(fontManager.getFont("CalliCat"), screenH * 0.04f);
        std::vector<float> positions = { screenH * 0.5f - offset, screenH * 0.6f - offset, screenH * 0.7f - offset };
        int hovered = hitTestMenuRow(mouse, positions, screenW * 0.38f, screenW * 0.7f, screenH * 0.07f);
        if (mouse.consumeMoved() && hovered >= 0) selector = hovered;
        bool clicked = mouse.consumeClick();

        if (kb.consumeMenuConfirm() || (clicked && hovered >= 0)) {
            switch (selector) {
                case 0:
                    onResume();
                    stack.pop();
                    break;
                case 1:
                    resetGame();
                    stack.pop();
                    break;
                case 2:
                    resetGame();
                    onMainMenu();
                default: break;

            }
        }
};

void PauseMenu::draw(RenderContext& tools) {
    tools.textRenderer.drawText("Pause", "CalliCat", tools.screenW * 0.43f, tools.screenH * 0.4f, tools.screenH * 0.08f, tools.fontManager.getFont("CalliCat"), 0.01f);
    float positions[] = { tools.screenH * 0.5f, tools.screenH * 0.6f, tools.screenH * 0.7f };
    Texture& pawTex = tools.textureManager.getOrLoad("cat_paw.png");
    float pawSize = tools.screenH * 0.04f;
    float baselineToVisualCenter = textBaselineToVisualCenter(tools.fontManager.getFont("CalliCat"), tools.screenH * 0.04f);
    tools.uiRenderer.drawUIComponent(pawTex, tools.screenW * 0.40f, positions[selector] - baselineToVisualCenter - pawSize * 0.5f, 0.0f, pawSize, pawSize);
    tools.textRenderer.drawText("Resume", "CalliCat", tools.screenW * 0.45f, tools.screenH * 0.5f, tools.screenH * 0.05f, tools.fontManager.getFont("CalliCat"), 0.01f);
    tools.textRenderer.drawText("Play again", "CalliCat", tools.screenW * 0.45f, tools.screenH * 0.6f, tools.screenH * 0.05f, tools.fontManager.getFont("CalliCat"), 0.01f);
    tools.textRenderer.drawText("Main Menu", "CalliCat", tools.screenW * 0.45f, tools.screenH * 0.7f, tools.screenH * 0.05f, tools.fontManager.getFont("CalliCat"), 0.01f);

    //tuto keys
    Texture& leftArrow = tools.textureManager.getOrLoad("keyboard_arrow_left_outline.png");
    Texture& rightArrow = tools.textureManager.getOrLoad("keyboard_arrow_right_outline.png");
    tools.uiRenderer.drawUIComponent(leftArrow, tools.screenW * 0.005f, tools.screenH * 0.92f, 0.0f, tools.screenH * 0.08f, tools.screenH * 0.08f);
    tools.uiRenderer.drawUIComponent(rightArrow, tools.screenW * 0.05f, tools.screenH * 0.92f, 0.0f, tools.screenH * 0.08f, tools.screenH * 0.08f);
    tools.textRenderer.drawText("Left/Right", "CalliCat", tools.screenW * 0.11f, tools.screenH * 0.97f, tools.screenH * 0.028f, tools.fontManager.getFont("CalliCat"), 0.01f);

    Texture& upArrow = tools.textureManager.getOrLoad("keyboard_arrow_up_outline.png");
    tools.uiRenderer.drawUIComponent(upArrow, tools.screenW * 0.25f, tools.screenH * 0.92f, 0.0f, tools.screenH * 0.08f, tools.screenH * 0.08f);
    tools.textRenderer.drawText("Jump", "CalliCat", tools.screenW * 0.31f, tools.screenH * 0.97f, tools.screenH * 0.028f, tools.fontManager.getFont("CalliCat"), 0.01f);
};