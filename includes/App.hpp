#pragma once
#include <iostream>
#include <Window.hpp>
#include <Renderer.hpp>
#include <Camera.hpp>
#include <Mesh.hpp>
#include <Skybox.hpp>
#include <Transform.hpp>
#include <Timer.hpp>
#include <Event.hpp>
#include <Keyboard.hpp>
#include <Mouse.hpp>

class App {
	private:
		Window window;
		Renderer renderer;
		Mesh mesh;
		Camera camera;
		Transform transform;
		Skybox skybox;
		Timer timer;
		Keyboard keyboard;
		Mouse mouse;

		bool running;
		bool mouselock = true;
		float fps;
		float deltaTime;

		void processEvents();
		void update();
		void render();
	public:
		App(int width, int height, const std::string& objPath);
		~App();
		void run();
		void FPScalculator();
};