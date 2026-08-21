#pragma once
#include <UI/OptionsMenu.hpp>
#include <UI/MenuScreen.hpp>
#include <UI/SkinMenu.hpp>
#include <UI/SkinInfo.hpp>
#include <stack>


class MainMenu : public MenuScreen {
    public:
        void update(Keyboard& kb, Mouse& mouse, FontManager& fontManager, int screenW, int screenH, std::stack<MenuScreen*>& stack);
        void draw(RenderContext& tools);
        MainMenu(std::function<void()> onPlay, std::function<void()> onQuit, std::function<void(const std::string&)> onSelectSkin, std::vector<SkinInfo>& skins, unsigned int& coinCount);
    private:
        std::function<void()> onPlay;
        std::function<void()> onQuit;
        std::function<void(const std::string&)> onSelectSkin;
        std::vector<SkinInfo>& skins;
        unsigned int& coinCount;
        int selector = 0;

};

