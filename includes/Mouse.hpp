#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <Camera.hpp>
#include <Renderer.hpp>
#include <Transform.hpp>


class Mouse {
	private:
		float rotX = 0.0f, rotY = 0.0f, wheel = 0.0f;
		bool rotating = false;
	public:
		~Mouse() {};
		void processEvent(SDL_Event& e);
		void applyRotation(Transform& trans, Camera& cam);
};