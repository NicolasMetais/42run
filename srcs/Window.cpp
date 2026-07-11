#include <Window.hpp>
#include <../loader/include/glad/glad.h>

Window::Window(int w, int h) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		throw SDLException(SDL_GetError());
	// MSAA 2x : a demander avant la creation de la fenetre
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	win = SDL_CreateWindow("42Run", 0, 0, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!win) {
		// certains drivers refusent le 4x : on retente en 2x puis sans MSAA
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
		win = SDL_CreateWindow("42Run", 0, 0, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	}
	if (!win) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
		win = SDL_CreateWindow("42Run", 0, 0, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	}
	if (!win)
		throw SDLException(SDL_GetError());
	context = SDL_GL_CreateContext(win);
	if (!context)
		throw SDLException(SDL_GetError());
	SDL_SetRelativeMouseMode(SDL_TRUE);
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    	throw std::runtime_error("Failed to initialize GLAD");
	}
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



