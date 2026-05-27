#pragma once
#include <UI/MenuScreen.hpp>
#include <stack>


class GameOverMenu : public MenuScreen {
    public:
        void update(Keyboard& kb, std::stack<MenuScreen*>& stack);
        void draw(RenderContext& tools);
        GameOverMenu(std::function<void()> resetGame, std::function<void()> onMainMenu);
    private:
        std::function<void()> resetGame;
        std::function<void()> onMainMenu;
        int selector = 0;
};

