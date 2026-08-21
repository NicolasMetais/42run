#pragma once
#include <iostream>
#include <SDL2/SDL.h>

/** @brief Thrown when an SDL call fails. */
class SDLException : public std::runtime_error {
	public:
		SDLException(const std::string& msg):
			std::runtime_error("SDL Error: " + msg) {}
};

/** @brief Creates and owns an SDL window with an OpenGL context. */
class Window {
	private:
		SDL_Window* win;
		SDL_GLContext context;
	public:
		Window();
		/** @brief Creates a window of the given dimensions and initialises the OpenGL context. */
		Window(int w, int h);
		SDL_Window* getWin();
		~Window();
};
