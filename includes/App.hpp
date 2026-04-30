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
#include <AnimationManager.hpp>

/** @brief Top-level application: owns all subsystems and drives the main loop. */
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
		AnimationManager animManager;
		std::vector<LoadedModel> models;
		bool running;
		bool mouselock = true;
		float fps;
		float deltaTime;

		/** @brief Polls and dispatches SDL events to input subsystems. */
		void processEvents();
		/** @brief Updates game state (camera, animations, transforms) for the current frame. */
		void update();
		/** @brief Issues draw calls for all loaded models and the skybox. */
		void render();
		/**
		 * @brief Recursively renders a node and its children using the scene graph.
		 * @param lm         The loaded model owning the node.
		 * @param nodeIdx    Index of the node to render.
		 * @param parentWorld World-space transform inherited from the parent node.
		 * @param view        View matrix from the camera.
		 * @param projection  Projection matrix.
		 */
		void renderNode(LoadedModel& lm, int nodeIdx, const Matrix<float>& parentWorld, const Matrix<float>& view, const Matrix<float>& projection);
	public:
		/** @brief Initialises the window, OpenGL context, and all subsystems at the given resolution. */
		App(int width, int height);
		~App();
		/** @brief Loads a GLTF or OBJ model from @p filename and adds it to the scene. */
		void LoadNewModel(std::string filename);
		/** @brief Starts the main loop; blocks until the application exits. */
		void run();
		/** @brief Computes and stores the current frames-per-second value. */
		void FPScalculator();
};
