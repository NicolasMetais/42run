#include <Window.hpp>
#include <../loader/include/glad/glad.h>

Window::Window(int w, int h) {
if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		throw SDLException(SDL_GetError());

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // optionnel

	win = SDL_CreateWindow("scop", 0, 0, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!win)
		throw SDLException(SDL_GetError());
	context = SDL_GL_CreateContext(win);
	if (!context)
		throw SDLException(SDL_GetError());
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_GL_SetSwapInterval(1);

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



