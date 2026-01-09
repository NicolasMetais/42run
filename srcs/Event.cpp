#include <Event.hpp>

void event(SDL_Event& e, Camera& camera, bool& run) {
	switch (e.type) {
		case SDL_QUIT:
			run = false;
		break;
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				int w = e.window.data1;
				int h = e.window.data2;
				glViewport(0, 0, w, h);
				camera.updateProjection(w, h);
			}
			break ;
	}
}