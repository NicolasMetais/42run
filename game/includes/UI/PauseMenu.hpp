#pragma once
#include <UI/MenuScreen.hpp>
#include <stack>


class PauseMenu : public MenuScreen {
    public:
        void update(Keyboard& kb, Mouse& mouse, FontManager& fontManager, int screenW, int screenH, std::stack<std::unique_ptr<MenuScreen>>& stack);
        void draw(RenderContext& tools);
        PauseMenu(std::function<void()> onResume, std::function<void()> resetGame, std::function<void()> onMainMenu);
    private:
        std::function<void()> onResume;
        std::function<void()> resetGame;
        std::function<void()> onMainMenu;
        int selector = 0;
};

