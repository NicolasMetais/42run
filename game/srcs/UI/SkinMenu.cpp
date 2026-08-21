#include <UI/SkinMenu.hpp>

SkinMenu::SkinMenu(std::function<void(const std::string&)> onSelectSkin, std::vector<SkinInfo>& skins, int& coinCount) : onSelectSkin(onSelectSkin), skins(skins), coinCount(coinCount) {};

void SkinMenu::update(Keyboard& kb, Mouse& mouse, FontManager& fontManager, int screenW, int screenH, std::stack<MenuScreen*>& stack) {
        int backIndex = (int)skins.size();

        if (kb.consumeMenuUp() && selector > 0)
            selector--;
        if (kb.consumeMenuDown() && selector < backIndex)
            selector++;

        float offset = textBaselineToVisualCenter(fontManager.getFont("CalliCat"), screenH * 0.04f);
        std::vector<float> positions;
        for (int i = 0; i <= backIndex; ++i)
            positions.push_back(screenH * (0.5f + 0.1f * i) - offset);
        int hovered = hitTestMenuRow(mouse, positions, screenW * 0.38f, screenW * 0.75f, screenH * 0.07f);
        if (mouse.consumeMoved() && hovered >= 0) selector = hovered;
        bool clicked = mouse.consumeClick();

        if (kb.consumeMenuConfirm() || (clicked && hovered >= 0)) {
            if (selector == backIndex) {
                stack.pop();
            } else {
                SkinInfo& skin = skins[selector];
                if (skin.owned || coinCount >= skin.price) {
                    if (!skin.owned) {
                        coinCount -= skin.price;
                        skin.owned = true;
                    }
                    for (auto& s : skins) s.equipped = false;
                    skin.equipped = true;
                    onSelectSkin(skin.file);
                }
            }
        }
};

void SkinMenu::draw(RenderContext& tools) {
    int backIndex = (int)skins.size();
    float offset = textBaselineToVisualCenter(tools.fontManager.getFont("CalliCat"), tools.screenH * 0.04f);
    std::vector<float> positions;
    for (int i = 0; i <= backIndex; ++i)
        positions.push_back(tools.screenH * (0.5f + 0.1f * i));

    tools.textRenderer.drawText("Select your skin :", "CalliCat", tools.screenW * 0.38f, tools.screenH * 0.4f, tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
    Texture& pawTex = tools.textureManager.getOrLoad("cat_paw.png");
    float pawSize = tools.screenH * 0.04f;
    tools.uiRenderer.drawUIComponent(pawTex, tools.screenW * 0.40f, positions[selector] - offset - pawSize * 0.5f, 0.0f, pawSize, pawSize);

    for (size_t i = 0; i < skins.size(); ++i) {
        float size = tools.screenH * 0.04f;
        float x = tools.screenW * 0.47f;
        if (skins[i].equipped) {
            tools.textRenderer.drawText(skins[i].label + " (equipped)", "CalliCat", x, positions[i], size, tools.fontManager.getFont("CalliCat"));
        } else if (!skins[i].owned) {
            // CalliCat n'a pas de vrais glyphes chiffres : le prix passe par Roboto, le reste par CalliCat
            x = tools.textRenderer.drawText(skins[i].label + " - ", "CalliCat", x, positions[i], size, tools.fontManager.getFont("CalliCat"));
            x = tools.textRenderer.drawText(std::to_string(skins[i].price), "Roboto", x, positions[i], size, tools.fontManager.getFont("Roboto"));
            tools.textRenderer.drawText(" coins", "CalliCat", x, positions[i], size, tools.fontManager.getFont("CalliCat"));
        } else {
            tools.textRenderer.drawText(skins[i].label, "CalliCat", x, positions[i], size, tools.fontManager.getFont("CalliCat"));
        }
    }
    tools.textRenderer.drawText("Back", "CalliCat", tools.screenW * 0.47f, positions[backIndex], tools.screenH * 0.04f, tools.fontManager.getFont("CalliCat"));
};
