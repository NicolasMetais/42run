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
		int posX = 0, posY = 0;
		bool clicked = false; // clic gauche en attente, consomme une seule fois
		bool moved = false; // mouvement en attente, consomme une seule fois
	public:
		~Mouse() {};
		/** @brief Reads mouse motion and button state from an SDL event. */
		void processEvent(SDL_Event& e);
		/** @brief Applies the accumulated rotation delta to the transform and camera. */
		void applyRotation(Transform& trans, Camera& cam);
		/** @brief Position actuelle de la souris en pixels ecran. */
		int getX() const { return posX; }
		int getY() const { return posY; }
		/** @brief Renvoie true une seule fois apres un clic gauche, puis se reinitialise. */
		bool consumeClick() { bool c = clicked; clicked = false; return c; }
		/** @brief Renvoie true une seule fois apres un mouvement, puis se reinitialise.
		 * Sert a ne pas laisser la souris immobile reprendre la main sur le clavier. */
		bool consumeMoved() { bool m = moved; moved = false; return m; }
};
