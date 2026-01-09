#pragma once
#include <iostream>
#include <Matrix/Matrix.hpp>
#include <SDL2/SDL.h>
#include <Mesh.hpp>

class Camera {
	private:
		Vector<float> cameraPos;
		Vector<float> target;
		Vector<float> up;
		Vector<float> U;
		Vector<float> V;
		Vector<float> N;
		Vector<float> mousePos;
		int winWidth;
		int winHeight;
		float aspect;
		float fov;
		float near;
		float far;
		float speed;
		float angleH;
		float angleV;
		bool upperEdge;
		bool leftEdge;
		bool rightEdge;
		bool lowerEdge;
	public:
		Camera(float w, float h, Vector<float> pos);
		Camera(float w, float h, Vector<float> pos, Vector<float> target, Vector<float> up);
		Matrix<float> buildProjection();
		Matrix<float> updateProjection(float w, float h);
		Matrix<float> buildView();
		Matrix<float> buildViewNoTranslation();
		void moveForward(float deltaTime);
		void moveBackward(float deltaTime);
		void moveLeft(float deltaTime);
		void moveRight(float deltaTime);
		void moveUp(float deltaTime);
		void moveDown(float deltaTime);
		void speedUp();
		void speedDown();
		void cameraInit();
		void cameraUpdate();
		void mouseActions();
		Vector<float> getMousePos() const { return this->mousePos; };
		Vector<float> getCameraPos() const { return this->cameraPos; };
		const Vector<float>& getTarget() const { return this->target; };
};