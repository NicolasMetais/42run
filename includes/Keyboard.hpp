#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <Camera.hpp>
#include <Renderer.hpp>
#include <Transform.hpp>

class Keyboard {
	private:
		uint16_t moveFlags = 0;
	public:
		~Keyboard() {};
		void processEvent(SDL_Event& e, bool& running, Renderer& render, Camera& cam, float& fps, bool& lockCam);
		void applyMovement(Camera& camera, Transform& transform, float deltaTime);
};