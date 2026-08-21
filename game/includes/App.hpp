#pragma once
#include <iostream>
#include <Window.hpp>
#include <stack>
#include <Renderer/Renderer.hpp>
#include <Camera.hpp>
#include <Loader/ObjImporter.hpp>
#include <Skybox/Skybox.hpp>
#include <Scene/Transform.hpp>
#include <Timer.hpp>
#include <Input/Event.hpp>
#include <Input/Keyboard.hpp>
#include <Texture/TextureManager.hpp>
#include <Scene/SceneManager.hpp>
#include <Loader/Scene.hpp>
#include <Input/Mouse.hpp>
#include <Loader/LoadedModel.hpp>
#include <Loader/ModelLoader.hpp>
#include <Scene/GameScene.hpp>
#include <Runner/ChunkManager.hpp>
#include <Animation/AnimationManager.hpp>
#include <Systems/PhysicsSystem.hpp>
#include <Systems/CollisionSystem.hpp>
#include <UI/UIRenderer.hpp>
#include <Font/FontManager.hpp>
#include <UI/MenuScreen.hpp>
#include <UI/GameOverMenu.hpp>
#include <UI/PauseMenu.hpp>
#include <UI/MainMenu.hpp>
#include <UI/SkinInfo.hpp>
#include <Font/TextRenderer.hpp>

enum class AppState { PLAYING, GAME_OVER, PAUSED };

// Indices des animations dans Cat.gltf (ordre d'export, pas de noms cote moteur)
constexpr int ANIM_JUMP = 0;
constexpr int ANIM_RUN  = 1;
constexpr float RUN_ANIM_SPEED = 0.6f; // le cycle exporte tourne trop vite vs le deplacement reel

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
		TextureManager textureManager;
		AnimationManager animManager;
		GameScene* activeScene = nullptr;
		std::vector<GameScene> scenes;
		bool running;
		bool mouselock = true;
		float fps;
		bool showFps = false;
		float fpsAccumTime = 0.0f;   ///< fenetre de lissage du compteur affiche
		int fpsAccumFrames = 0;
		int fpsDisplay = 0;
		float deltaTime;
		float distance;
		int coinCount = 0; ///< portefeuille : credite en temps reel a chaque ramassage, jamais remis a 0 (meme a la mort)
		std::vector<SkinInfo> skins = {
			{"Moulinette.gltf", "Moulinette", 0,   true,  true},
			{"BlackCat.gltf",   "Black Cat",  100, false, false},
			{"OrangeCat.gltf",  "Golden Cat", 150, false, false},
		}; // prix a ajuster
		std::vector<EntityId> pendingPickups; ///< coins ramasses cette frame, detruits apres resolveEntities (pas pendant, la map colliders est en cours d'iteration)
		static constexpr float COIN_SPIN_SPEED = 3.0f; ///< radians/s, a ajuster
		ModelLoader modelLoader;
		std::optional<ChunkManager> chunkManager;
		TextRenderer textRenderer;
		FontManager fontManager;
		AppState state = AppState::PLAYING;
		UIRenderer uiRenderer;
		std::vector<RippleDrop> ripples;
		std::vector<std::pair<float, EntityId>> byDistance; ///< buffer reutilise pour le tri des transparents (evite une reallocation par frame)
		int screenW;
		int screenH;
		float prevMouseU = 0.0f;
		float prevMouseV = 0.0f;
		float elapsedTime = 0.0f;
		float lastDropTime = 0.0f;
		int selectedOption = 0;
		int lanePosition = Lane::COUNT / 2;
		int lastLanePosition = lanePosition; ///< valeur d'avant la derniere tentative de changement de lane, pour le rejet "bump"
		std::stack<MenuScreen*> menus;
		RenderContext menuContext;
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
		void renderNode(LoadedModel& lm, int nodeIdx, const Matrix<float>& parentWorld, const Matrix<float>& view, const Matrix<float>& projection, const std::unordered_set<int>& hiddenNodes, RenderPass pass);
		/** @brief Draws the 3D game world (entities, skybox, transparents, score) for the given view/projection. */
		void drawGameWorld(const Matrix<float>& view, Matrix<float>& projection);
		float menuFadeTime = 0.0f; ///< temps ecoule depuis le game over/la pause, pour le fondu au noir derriere le menu
		static constexpr float MENU_FADE_DURATION = 0.6f;
		static constexpr float MENU_FADE_MAX_DARKNESS = 0.7f;
	public:
		/** @brief Initialises the window, OpenGL context, and all subsystems at the given resolution. */
		App(int width, int height);
		~App();
		/** @brief Starts the main loop; blocks until the application exits. */
		void run();
		/** @brief Computes and stores the current frames-per-second value. */
		void FPScalculator();
		void resetGame();
		void triggerGameOver();
		void triggerPause();
};
