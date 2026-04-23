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
#include <TextureManager.hpp>
#include <SceneManager.hpp>
#include <Scene.hpp>
#include <Mouse.hpp>
#include <LoadedModel.hpp>

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
		TextureManager textureManager;
		SceneManager sceneManager;
		std::vector<LoadedModel> models;
		bool running;
		bool mouselock = true;
		float fps;
		float deltaTime;
		void processEvents();
		void update();
		void render();
		void renderNode(LoadedModel& lm, int nodeIdx, const Matrix<float>& parentWorld, const Matrix<float>& view, const Matrix<float>& projection);
	public:
		App(int width, int height);
		~App();
		void LoadNewModel(std::string filename);
		void run();
		void FPScalculator();
};