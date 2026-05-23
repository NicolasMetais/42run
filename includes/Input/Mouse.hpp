#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <Camera.hpp>
#include <Renderer/Renderer.hpp>
#include <Scene/Transform.hpp>

/** @brief Handles mouse input: tracks motion and applies rotation to the transform/camera. */
class Mouse {
	private:
		float rotX = 0.0f, rotY = 0.0f, wheel = 0.0f;
		bool rotating = false;
	public:
		~Mouse() {};
		/** @brief Reads mouse motion and button state from an SDL event. */
		void processEvent(SDL_Event& e);
		/** @brief Applies the accumulated rotation delta to the transform and camera. */
		void applyRotation(Transform& trans, Camera& cam);
};
