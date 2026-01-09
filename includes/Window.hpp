#pragma once
#include <iostream>
#include <SDL2/SDL.h>


class SDLException : public std::runtime_error {
	public:
		SDLException(const std::string& msg):
			std::runtime_error("SDL Error: " + msg) {}
};

class Window {
	private:
		SDL_Window* win;
		SDL_GLContext context;
	public:
		Window();
		Window(int w, int h);
		SDL_Window* getWin();
		~Window();
};