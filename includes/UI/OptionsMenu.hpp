#pragma once
#include <UI/OptionsMenu.hpp>
#include <UI/MenuScreen.hpp>
#include <stack>


class OptionsMenu : public MenuScreen {
    public:
        void update(Keyboard& kb, std::stack<MenuScreen*>& stack);
        void draw(RenderContext& tools);
    private:
        int selector = 0;
};

