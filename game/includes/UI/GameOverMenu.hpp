#pragma once
#include <UI/MenuScreen.hpp>
#include <stack>


class GameOverMenu : public MenuScreen {
    public:
        void update(Keyboard& kb, Mouse& mouse, FontManager& fontManager, int screenW, int screenH, std::stack<std::unique_ptr<MenuScreen>>& stack);
        void draw(RenderContext& tools);
        GameOverMenu(std::function<void()> resetGame, std::function<void()> onMainMenu, float& distance);
    private:
        std::function<void()> resetGame;
        std::function<void()> onMainMenu;
        float& distance;
        int selector = 0;
};

