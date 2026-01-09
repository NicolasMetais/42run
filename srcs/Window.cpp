#include <Window.hpp>

Window::Window(int w, int h) {
if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		throw SDLException(SDL_GetError());
	win = SDL_CreateWindow("scop", 0, 0, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!win)
		throw SDLException(SDL_GetError());
	context = SDL_GL_CreateContext(win);
	if (!context)
		throw SDLException(SDL_GetError());
	SDL_SetRelativeMouseMode(SDL_TRUE);
};

Window::~Window() {
	if (win)
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(win);
	}
	SDL_Quit();
};

SDL_Window* Window::getWin() {
	return win;
}



