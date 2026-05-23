#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <Camera.hpp>
#include <Renderer/Renderer.hpp>
#include <Scene/Transform.hpp>

/** @brief Processes a single SDL event and updates the camera or running state accordingly. */
void event(SDL_Event& e, Camera& camera, bool& run);
