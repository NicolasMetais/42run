#pragma once
#include <iostream>
#include <Matrix/Matrix.hpp>

/** @brief World-space transform: position, Euler rotation, and uniform scale. */
class Transform {
	private:
		Vector<float> position;
		Vector<float> rotation; ///< Euler angles in radians (X, Y, Z).
		float scale;
	public:
		Transform(): position{0,0,0}, rotation{0,0,0}, scale(1) {};

		void setPosition(float x, float y, float z) { this->position = Vector<float>{x,y,z}; };
		const Vector<float>& getPosition() const { return position; };
		void setRotate(float x, float y, float z)   { this->rotation = Vector<float>{x,y,z}; };
		/** @brief Adds @p (x,y,z) to the current Euler rotation angles. */
		void addRotation(float x, float y, float z);
		void setScale(float scale) { this->scale = scale; };
		float getScale() const { return this->scale; };
		/** @brief Translates the position by @p delta. */
		void move(const Vector<float>& delta);
		/** @brief Builds and returns the TRS model matrix from position, rotation and scale. */
		Matrix<float> getModelMatrix();
};
