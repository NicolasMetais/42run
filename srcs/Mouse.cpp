#include <Mouse.hpp>

#define SENSITIVITY 0.01f

void Mouse::processEvent(SDL_Event& e) {
	switch (e.type) {
				case SDL_MOUSEBUTTONDOWN:
			if (e.button.button == SDL_BUTTON_LEFT)
				this->rotating = true;
		break ;
		case SDL_MOUSEBUTTONUP:
			if (e.button.button == SDL_BUTTON_LEFT)
				this->rotating = false;
		break ;
		case SDL_MOUSEMOTION:
			if (this->rotating) {
				this->rotX += static_cast<float>(e.motion.xrel);
				this->rotY += static_cast<float>(e.motion.yrel);
			}
		break ;
		case SDL_MOUSEWHEEL:
		if (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
			this->wheel -= e.wheel.y;
		else
			this->wheel += e.wheel.y;
		break ;
	}
};

void Mouse::applyRotation(Transform& transform, Camera& cam) {
	if (this->rotating) {
		transform.addRotation(this->rotY * SENSITIVITY, this->rotX * SENSITIVITY, 0.0f);
		int dummyX, DummyY;
		SDL_GetRelativeMouseState(&dummyX, &DummyY);
		rotX = 0.0f;
		rotY = 0.0f;
	}
	else
		cam.mouseActions();
	if (this->wheel != 0.0f) {
		float scale = transform.getScale();
		scale += this->wheel * 0.1f;
		if (scale < 0.1f) scale = 0.1f;
		if (scale > 8.0f) scale = 8.0f;
		transform.setScale(scale);
		this->wheel = 0.0f;
	}
};