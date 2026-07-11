#include <App.hpp>
#include <algorithm>

App::App(int width, int height) 
        : window(width, height), renderer(), mesh(), camera(static_cast<float>(width), static_cast<float>(height)
        , Vector<float>{0, 1, 3}, Vector<float>{0,0,0}, Vector<float>{0,1,0}), running(true)
        , modelLoader(renderer, textureManager), textRenderer(textureManager, width, height)
        , uiRenderer(textureManager, width, height), screenW(width), screenH(height)
        , menuContext(textRenderer, fontManager, uiRenderer, screenW, screenH) {
    this->skybox.generateIrradianceMap();
    this->skybox.generatePrefilterMap();

    scenes.push_back(GameScene::fromJson("resources/levels/menu.json", modelLoader));
    scenes.push_back(GameScene::fromJson("resources/levels/level1.json", modelLoader));
    activeScene = &scenes[0];
    chunkManager.emplace(*activeScene, modelLoader, Lane::COUNT, 7.0f, 5.0f, "resources/Chunk.gltf", std::vector<std::string>{"resources/Obstacles.gltf", "resources/screens.gltf"}, [this]() { this->triggerGameOver(); } );
    activeScene->loadPlayer("resources/player.json", modelLoader);
    activeScene->transforms[activeScene->playerId].setPosition(Lane::centerX(lanePosition), 0, 0);
    fontManager.load(this->textureManager, "resources/Roboto.json", "Roboto");
    menus.push(new MainMenu(
        [this]() { menus.pop(); this->distance = 0.0f; },
        [this]() { running = false;}
    ));
	// this->transform.setScale(1.0f);
	// Vector<float> cent(3);
	// cent = (data.max + data.min) * 0.5f;
	// this->transform.setPosition(-cent.x(), -cent.y(), -cent.z());
};

App::~App(){};

void App::update() {
    this->elapsedTime += this->deltaTime;
    if (menus.empty() && state == AppState::PLAYING) {
        // dans update(), à la place de gametime :
        distance += chunkManager->getRunspeed() * deltaTime;
        chunkManager->update(deltaTime);
        if (activeScene->playerId != UINT32_MAX) {
            auto& rb = activeScene->rigidbodies[activeScene->playerId];
            constexpr float SPEED = 5.0f;
            rb.velocity.x() = 0.0f;
            rb.velocity.z() = 0.0f;
            if (keyboard.consumeLeft() && lanePosition < Lane::COUNT - 1) this->lanePosition++;
            if (keyboard.consumeRight() && lanePosition > 0) this->lanePosition--;
            if (keyboard.isForward()) rb.velocity.z() =  SPEED;
            if (keyboard.isBack())    rb.velocity.z() = -SPEED;
            if (keyboard.isJump() && rb.onGround) rb.velocity.y() = 8.0f;
            float targetX = Lane::centerX(lanePosition);
            float currentX = activeScene->transforms[activeScene->playerId].getPosition().x();
            activeScene->transforms[activeScene->playerId].move({(targetX - currentX) * SPEED * deltaTime, 0, 0});
        }

        PhysicsSystem::update(activeScene->transforms, activeScene->rigidbodies, deltaTime);
        CollisionSystem::resolveEntities(activeScene->transforms, activeScene->colliders, activeScene->rigidbodies, activeScene->triggers);

        if (activeScene->playerId != UINT32_MAX)
            camera.follow(activeScene->transforms[activeScene->playerId].getPosition());

        for (auto& [id, render] : activeScene->renders)
            animManager.update(*render.model, deltaTime);
    }
};

void App::processEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		event(e, this->camera, this->running);
		mouse.processEvent(e);
        if (!menus.empty())
		    keyboard.processMenuEvent(e, this->running);
        else
		    keyboard.processEvent(e, this->running, this->camera, this->fps, this->mouselock);
		// if (e.type == SDL_KEYDOWN) {
		// 	SDL_Keycode sym = e.key.keysym.sym;
		// 	SDL_Scancode sc  = e.key.keysym.scancode;
		// 	if (sc == SDL_SCANCODE_1 || sym == SDLK_1 || sym == SDLK_KP_1) animManager.setAnimation(0);
		// 	if (sc == SDL_SCANCODE_2 || sym == SDLK_2 || sym == SDLK_KP_2 || sc == SDL_SCANCODE_E) animManager.setAnimation(1);
		// 	if (sc == SDL_SCANCODE_3 || sym == SDLK_3 || sym == SDLK_KP_3) animManager.setAnimation(2);
		// }
	}
	// mouse.applyRotation(this->transform, this->camera); //mouse rotation
	keyboard.applyMovement(this->camera, this->transform, this->deltaTime); //keyboard movement
};

void App::renderNode(LoadedModel& lm, int nodeIdx, const Matrix<float>& parentWorld,
                     const Matrix<float>& view, const Matrix<float>& projection,
                     const std::unordered_set<int>& hiddenNodes, RenderPass pass) {
	if (hiddenNodes.count(nodeIdx)) return;
	const Node& node = lm.gltf.nodes[nodeIdx];
	Matrix<float> world = parentWorld * utils::nodeLocalMatrix(node);
	Matrix<float> mvp = projection * view * world;

	if (node.mesh >= 0 && node.mesh < (int)lm.meshes.size()) {
		const std::vector<Matrix<float>>* jointMats = nullptr;
		if (node.skin >= 0 && node.skin < (int)lm.jointMatrices.size())
			jointMats = &lm.jointMatrices[node.skin];
		renderer.rendering(mvp, lm.meshes[node.mesh], world, camera, skybox.getIrradianceMapId(), skybox.getPrefilterMapId(), jointMats, pass);
	}

	for (int child : node.children)
		renderNode(lm, child, world, view, projection, hiddenNodes, pass);
};

void App::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Matrix<float> view = camera.buildView();
    Matrix<float> projection = camera.buildProjection();
    if (!menus.empty()) {
        int mX, mY;
        SDL_GetMouseState(&mX, &mY);
        float mouseU = (float)mX / this->screenW;
        float mouseV = (float)mY / this->screenH;
        (void)mouseU;
        (void)mouseV;
        camera.rotateH(deltaTime * 10.0f);
        skybox.draw(camera.buildViewNoTranslation(), projection);
        
            menus.top()->update(keyboard, menus);
        if (!menus.empty())
            menus.top()->draw(menuContext);
    } else if (state == AppState::PLAYING) {
        auto drawEntity = [&](EntityId id, RenderComponent& render, RenderPass pass) {
            LoadedModel& lm = *render.model;
            const Scene& scene = lm.gltf.scenes[lm.gltf.defaultScene];
            Matrix<float> modelMat = activeScene->transforms[id].getModelMatrix() * transform.getModelMatrix();
            for (int rootIdx : scene.rootNodes)
                renderNode(lm, rootIdx, modelMat, view, projection, render.hiddenNodes, pass);
        };

        // 1. opaques + MASK
        renderer.beginOpaquePass();
        for (auto& [id, render] : activeScene->renders)
            drawEntity(id, render, RenderPass::Opaque);

        // 2. skybox : remplit les pixels restants avant les transparents
        skybox.draw(camera.buildViewNoTranslation(), projection);

        // 3. transparents (BLEND) : tries loin -> proche, profondeur gelee
        std::vector<std::pair<float, EntityId>> byDistance;
        Vector<float> camPos = camera.getCameraPos();
        for (auto& [id, render] : activeScene->renders) {
            Vector<float> d = activeScene->transforms[id].getPosition() - camPos;
            byDistance.push_back({d.dot(d), id});
        }
        std::sort(byDistance.begin(), byDistance.end(),
                  [](const auto& a, const auto& b) { return a.first > b.first; });
        renderer.beginTransparentPass();
        for (auto& [dist, id] : byDistance)
            drawEntity(id, activeScene->renders[id], RenderPass::Transparent);
        renderer.endTransparentPass();

        std::string score = std::to_string((int)(distance));
        textRenderer.drawText(score + " Meters", "Roboto", 20, 40, 32, fontManager.getFont("Roboto"));
    }
    SDL_GL_SwapWindow(window.getWin());
};

void App::run(){
    SDL_GL_SetSwapInterval(0);
    timer.tick(); // discard loading time

    while(this->running)
    {
        this->deltaTime = timer.tick();
        if (this->deltaTime > 0.1f) this->deltaTime = 0.1f;
        processEvents();
        update();
        render();
        FPScalculator();
    }
    for (auto& [id, render] : activeScene->renders)
        for (auto& mesh : render.model->meshes)
            renderer.cleanup(mesh);
};

void App::FPScalculator() {
	this->fps = 1.0f / this->deltaTime;
};

void App::resetGame() {
    this->lanePosition = Lane::COUNT / 2;
    distance = 0.0f;

    activeScene->rigidbodies[activeScene->playerId].velocity = {0,0,0};
    activeScene->transforms[activeScene->playerId].setPosition(Lane::centerX(lanePosition), 0, 0); //pour virer l'animation de transition


    chunkManager.emplace(*activeScene, modelLoader, Lane::COUNT, 7.0f, 5.0f, "resources/Chunk.gltf", std::vector<std::string>{"resources/Obstacles.gltf", "resources/screens.gltf"}, [this]() { this->triggerGameOver(); });
};

void App::triggerGameOver() {
    menus.push(new GameOverMenu (
        [this]() { resetGame(); },
        [this]() {
            while (!menus.empty())
                menus.pop();
            menus.push(new MainMenu(
                [this]() { menus.pop(); },
                [this]() { running = false;}
            ));
        }
    ));
};

