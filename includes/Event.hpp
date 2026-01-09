#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <Camera.hpp>
#include <Renderer.hpp>
#include <Transform.hpp>

void event(SDL_Event& e,Camera& camera, bool& run);