#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <Camera.hpp>
#include <Renderer/Renderer.hpp>
#include <Scene/Transform.hpp>

/** @brief Handles keyboard input: records pressed keys and applies movement each frame. */
class Keyboard {
	private:
		uint32_t moveFlags = 0; ///< Bitmask of currently held movement keys.
	public:
		~Keyboard() {};
		/**
		 * @brief Processes a single SDL event, toggling move flags or triggering immediate actions.
		 * @param e       The SDL event to inspect.
		 * @param running Set to false to request application exit (e.g. on Escape).
		 * @param cam     Camera to pass speed changes to.
		 * @param fps     Current FPS value, displayed on key press.
		 * @param lockCam Toggled on mouse lock/unlock key press.
		 */
		void processEvent(SDL_Event& e, bool& running, Camera& cam, float& fps, bool& lockCam);
		void processMenuEvent(SDL_Event& e, bool& running);
		/** @brief Applies camera movement from WASD/Shift/Ctrl keys. */
		void applyMovement(Camera& camera, Transform& transform, float deltaTime);

		bool consumeLeft();
		bool consumeRight();
		bool isForward() const;
		bool isBack()    const;
		bool isJump()    const;
		bool consumeMenuUp();
		bool consumeMenuDown();
		bool consumeMenuConfirm();
		bool consumePause();
		/** @brief Efface toutes les touches actuellement enregistrees comme maintenues.
		 * A appeler aux transitions vers/depuis le gameplay : processEvent et processMenuEvent
		 * se partagent moveFlags mais ne connaissent chacun que leurs propres touches, donc un
		 * KEYUP qui atterrit dans le mauvais handler (touche relachee juste apres l'ouverture
		 * d'un menu) peut laisser un bit bloque a 1 indefiniment. */
		void reset() { moveFlags = 0; }
};
