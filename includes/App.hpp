#pragma once
#include <iostream>
#include <Window.hpp>
#include <Renderer.hpp>
#include <Camera.hpp>
#include <ObjImporter.hpp>
#include <Skybox.hpp>
#include <Transform.hpp>
#include <Timer.hpp>
#include <Event.hpp>
#include <Keyboard.hpp>
#include <GltfImporter.hpp>
#include <Mouse.hpp>

class App {
	private:
		Window window;
		Renderer renderer;
		ObjImporter mesh;
		Camera camera;
		Transform transform;
		Skybox skybox;
		Timer timer;
		Keyboard keyboard;
		Mouse mouse;
		GltfImporter gltf;
		MeshData data; //temporaire
		bool running;
		bool mouselock = true;
		float fps;
		float deltaTime;

		void processEvents();
		void update();
		void render();
	public:
		App(int width, int height);
		~App();
		void run();
		void FPScalculator();
};