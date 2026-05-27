#pragma once
#include <UI/OptionsMenu.hpp>
#include <UI/MenuScreen.hpp>
#include <UI/SkinMenu.hpp>
#include <stack>


class MainMenu : public MenuScreen {
    public:
        void update(Keyboard& kb, std::stack<MenuScreen*>& stack);
        void draw(RenderContext& tools);
        MainMenu(std::function<void()> onPlay, std::function<void()> onQuit);
    private:
        std::function<void()> onPlay;
        std::function<void()> onQuit;
        int selector = 0;

};

