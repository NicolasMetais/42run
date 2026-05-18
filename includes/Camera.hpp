#pragma once
#include <iostream>
#include <Matrix/Matrix.hpp>
#include <SDL2/SDL.h>
#include <ObjImporter.hpp>

/** @brief First-person perspective camera with mouse-look and keyboard movement. */
class Camera {
	private:
		Vector<float> cameraPos;
		Vector<float> target;
		Vector<float> up;
		Vector<float> U;       ///< Right basis vector of the camera frame.
		Vector<float> V;       ///< Up basis vector of the camera frame.
		Vector<float> N;       ///< Forward basis vector of the camera frame.
		Vector<float> mousePos;
		int winWidth;
		int winHeight;
		float aspect;
		float fov;
		float near;
		float far;
		float speed;
		float angleH;          ///< Horizontal look angle in radians.
		float angleV;          ///< Vertical look angle in radians.
		bool upperEdge;
		bool leftEdge;
		bool rightEdge;
		bool lowerEdge;
	public:
		/** @brief Constructs a camera at @p pos looking toward the origin. */
		Camera(float w, float h, Vector<float> pos);
		/** @brief Constructs a camera with explicit position, target and up vector. */
		Camera(float w, float h, Vector<float> pos, Vector<float> target, Vector<float> up);

		/** @brief Builds a perspective projection matrix from the current fov/aspect/near/far. */
		Matrix<float> buildProjection();
		/** @brief Rebuilds the projection matrix after a window resize. */
		Matrix<float> updateProjection(float w, float h);
		/** @brief Builds the view matrix from the current camera position and orientation. */
		Matrix<float> buildView();
		/** @brief Builds a view matrix with the translation component removed (used for skybox rendering). */
		Matrix<float> buildViewNoTranslation();

		/** @brief Moves the camera forward along its look direction. */
		void moveForward(float deltaTime);
		/** @brief Moves the camera backward along its look direction. */
		void moveBackward(float deltaTime);
		/** @brief Strafes the camera to the left. */
		void moveLeft(float deltaTime);
		/** @brief Strafes the camera to the right. */
		void moveRight(float deltaTime);
		/** @brief Moves the camera upward along the world up axis. */
		void moveUp(float deltaTime);
		/** @brief Moves the camera downward along the world up axis. */
		void moveDown(float deltaTime);
		/** @brief Increases camera movement speed. */
		void speedUp();
		/** @brief Decreases camera movement speed. */
		void speedDown();

		/** @brief Resets camera position and orientation to defaults. */
		void cameraInit();
		/** @brief Recomputes basis vectors (U, V, N) from the current angles. */
		void cameraUpdate();
		/** @brief Applies mouse delta to horizontal and vertical look angles. */
		void mouseActions();
		/**
		 * @brief Repositions the camera so the given bounding sphere is fully visible.
		 * @param center World-space center of the sphere.
		 * @param radius Radius of the bounding sphere.
		 */
		void fitToScene(const Vector<float>& center, float radius);

		void follow(const Vector<float>& playerPos);

		Vector<float> getMousePos() const { return this->mousePos; };
		Vector<float> getCameraPos() const { return this->cameraPos; };
		const Vector<float>& getTarget() const { return this->target; };
};
