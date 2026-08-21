#include <UI/MainMenu.hpp>

MainMenu::MainMenu(std::function<void()> onPlay, std::function<void()> onQuit, std::function<void(const std::string&)> onSelectSkin, std::vector<SkinInfo>& skins, int& coinCount) :onPlay(onPlay), onQuit(onQuit), onSelectSkin(onSelectSkin), skins(skins), coinCount(coinCount) {};

void MainMenu::update(Keyboard& kb, Mouse& mouse, FontManager& fontManager, int screenW, int screenH, std::stack<MenuScreen*>& stack) {
        if (kb.consumeMenuUp() && selector > 0)
            selector--;
        if (kb.consumeMenuDown() && selector < 2)
            selector++;

        float offset = textBaselineToVisualCenter(fontManager.getFont("CalliCat"), screenH * 0.05f);
        std::vector<float> positions = { screenH * 0.48f - offset, screenH * 0.6f - offset, screenH * 0.72f - offset };
        int hovered = hitTestMenuRow(mouse, positions, screenW * 0.38f, screenW * 0.75f, screenH * 0.08f);
        if (mouse.consumeMoved() && hovered >= 0) selector = hovered;
        bool clicked = mouse.consumeClick();

        if (kb.consumeMenuConfirm() || (clicked && hovered >= 0)) {
            switch (selector) {
                case 0:
                    onPlay(); break;
                case 1:
                    stack.push(new SkinMenu(onSelectSkin, skins, coinCount)); break ;
                case 2:
                    onQuit(); break;
                default: break;

            }
        }
};

void MainMenu::draw(RenderContext& tools) {
    tools.textRenderer.drawText("42", "Roboto", tools.screenW * 0.39f, tools.screenH * 0.3f, tools.screenH * 0.12f, tools.fontManager.getFont("Roboto"));
    tools.textRenderer.drawText("run", "CalliCat", tools.screenW * 0.49f, tools.screenH * 0.3f, tools.screenH * 0.12f, tools.fontManager.getFont("CalliCat"));
    float positions[] = { tools.screenH * 0.48f, tools.screenH * 0.6f, tools.screenH * 0.72f };
    Texture& pawTex = tools.textureManager.getOrLoad("cat_paw.png");
    float pawSize = tools.screenH * 0.05f;
    float baselineToVisualCenter = textBaselineToVisualCenter(tools.fontManager.getFont("CalliCat"), tools.screenH * 0.05f);
    tools.uiRenderer.drawUIComponent(pawTex, tools.screenW * 0.40f, positions[selector] - baselineToVisualCenter - pawSize * 0.5f, 0.0f, pawSize, pawSize);
    tools.textRenderer.drawText("Play", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.48f, tools.screenH * 0.06f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Skins", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.6f, tools.screenH * 0.06f, tools.fontManager.getFont("CalliCat"));
    tools.textRenderer.drawText("Quit", "CalliCat", tools.screenW * 0.47f, tools.screenH * 0.72f, tools.screenH * 0.06f, tools.fontManager.getFont("CalliCat"));

    //tuto keys
    Texture& leftArrow = tools.textureManager.getOrLoad("keyboard_arrow_left_outline.png");
    Texture& rightArrow = tools.textureManager.getOrLoad("keyboard_arrow_right_outline.png");
    tools.uiRenderer.drawUIComponent(leftArrow, tools.screenW * 0.005f, tools.screenH * 0.92f, 0.0f, tools.screenH * 0.08f, tools.screenH * 0.08f);
    tools.uiRenderer.drawUIComponent(rightArrow, tools.screenW * 0.05f, tools.screenH * 0.92f, 0.0f, tools.screenH * 0.08f, tools.screenH * 0.08f);
    tools.textRenderer.drawText("Left/Right", "CalliCat", tools.screenW * 0.11f, tools.screenH * 0.97f, tools.screenH * 0.028f, tools.fontManager.getFont("CalliCat"));

    Texture& upArrow = tools.textureManager.getOrLoad("keyboard_arrow_up_outline.png");
    tools.uiRenderer.drawUIComponent(upArrow, tools.screenW * 0.25f, tools.screenH * 0.92f, 0.0f, tools.screenH * 0.08f, tools.screenH * 0.08f);
    tools.textRenderer.drawText("Jump", "CalliCat", tools.screenW * 0.31f, tools.screenH * 0.97f, tools.screenH * 0.028f, tools.fontManager.getFont("CalliCat"));
};