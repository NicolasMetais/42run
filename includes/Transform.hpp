#pragma once
#include <iostream>
#include <Matrix/Matrix.hpp>


class Transform {
	private:
		Vector<float> position;
		Vector<float> rotation;
		float scale;
	public:
		Transform (): position{0,0,0}, rotation{0,0,0}, scale(1) {};
		void setPosition(float x, float y, float z) { this->position = Vector<float>{x,y,z}; };
		void setRotate(float x, float y, float z)  { this->rotation = Vector<float>{x,y,z}; };
		void addRotation(float x, float y, float z);
		void setScale(float scale) { this->scale = scale; };
		float getScale() { return this->scale; };
		void move(const Vector<float>& delta);
		Matrix<float> getModelMatrix();
};