#pragma once
#include <UI/MenuScreen.hpp>
#include <UI/SkinInfo.hpp>
#include <stack>

class SkinMenu : public MenuScreen {
    public:
        void update(Keyboard& kb, Mouse& mouse, FontManager& fontManager, int screenW, int screenH, std::stack<std::unique_ptr<MenuScreen>>& stack);
        void draw(RenderContext& tools);
        SkinMenu(std::function<void(const std::string&)> onSelectSkin, std::vector<SkinInfo>& skins, unsigned int& coinCount);
    private:
        std::function<void(const std::string&)> onSelectSkin;
        std::vector<SkinInfo>& skins;
        unsigned int& coinCount;
        int selector = 0;

};
