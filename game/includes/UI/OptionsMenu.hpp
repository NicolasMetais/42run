#pragma once
#include <UI/MenuScreen.hpp>
#include <stack>


class OptionsMenu : public MenuScreen {
    public:
        void update(Keyboard& kb, Mouse& mouse, FontManager& fontManager, int screenW, int screenH, std::stack<MenuScreen*>& stack);
        void draw(RenderContext& tools);
    private:
        int selector = 0;
};

