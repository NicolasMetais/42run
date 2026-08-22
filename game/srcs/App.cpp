#include <App.hpp>
#include <algorithm>
#include <limits>

App::App(int width, int height) 
        : window(width, height), renderer(), mesh(), camera(static_cast<float>(width), static_cast<float>(height)
        , Vector<float>{0, 1, 3}, Vector<float>{0,0,0}, Vector<float>{0,1,0}), running(true)
        , modelLoader(renderer, textureManager), textRenderer(textureManager, width, height)
        , uiRenderer(textureManager, width, height), screenW(width), screenH(height)
        , menuContext(textRenderer, fontManager, uiRenderer, textureManager, screenW, screenH) {
    this->skybox.generateIrradianceMap();
    this->skybox.generatePrefilterMap();
    glViewport(0, 0, width, height); // les deux appels precedents laissent le viewport bloque sur leur derniere taille de mip offscreen (16x16)

    scenes.push_back(GameScene::fromJson("resources/levels/menu.json", modelLoader));
    scenes.push_back(GameScene::fromJson("resources/levels/level1.json", modelLoader));
    activeScene = &scenes[0];
    chunkManager.emplace(*activeScene, modelLoader, Lane::COUNT, 7.35f, 5.0f, "resources/Chunk.gltf", std::vector<std::string>{"resources/Obstacles.gltf", "resources/screens.gltf", "resources/RandomVents.gltf", "resources/coin.gltf"}, [this]() { this->triggerGameOver(); }, [this]() { this->lanePosition = this->lastLanePosition; }, [this](EntityId id) { this->pendingPickups.push_back(id); } );
    activeScene->loadPlayer("resources/player.json", modelLoader);
    loadSave(); // ecrase le skin de player.json par celui sauvegarde, si une sauvegarde existe
    activeScene->transforms[activeScene->playerId].setPosition(Lane::centerX(lanePosition), 0, 0);
    animManager.setAnimation(ANIM_RUN, RUN_ANIM_SPEED); // 0=jump, 1=run-cycle
    fontManager.load(this->textureManager, "resources/CalliCat.json", "CalliCat");
    fontManager.load(this->textureManager, "resources/Roboto.json", "Roboto");
    menus.push(std::make_unique<MainMenu>(
        [this]() { keyboard.reset(); auto self = std::move(menus.top()); menus.pop(); this->distance = 0.0f; },
        [this]() { running = false;},
        [this](const std::string& model) { activeScene->setPlayerModel(model, modelLoader); writeSave(); },
        skins, coinCount
    ));
	// this->transform.setScale(1.0f);
	// Vector<float> cent(3);
	// cent = (data.max + data.min) * 0.5f;
	// this->transform.setPosition(-cent.x(), -cent.y(), -cent.z());
};

App::~App(){ writeSave(); };

void App::loadSave() {
    nlohmann::json data = SaveSystem::load(SAVE_PATH);
    coinCount = data.value("coins", coinCount);
    if (data.contains("skins") && data["skins"].is_object()) {
        for (auto& skin : skins) {
            if (!data["skins"].contains(skin.file)) continue;
            auto& s = data["skins"][skin.file];
            skin.owned = s.value("owned", skin.owned);
            skin.equipped = s.value("equipped", skin.equipped);
        }
    }
    for (auto& skin : skins)
        if (skin.equipped)
            activeScene->setPlayerModel(skin.file, modelLoader);
}

void App::writeSave() {
    nlohmann::json data;
    data["coins"] = coinCount;
    for (auto& skin : skins) {
        data["skins"][skin.file]["owned"] = skin.owned;
        data["skins"][skin.file]["equipped"] = skin.equipped;
    }
    SaveSystem::save(SAVE_PATH, data);
}

void App::update() {
    this->elapsedTime += this->deltaTime;
    if (menus.empty() && state == AppState::PLAYING && keyboard.consumePause()) {
        triggerPause();
        return;
    }
    if (menus.empty() && state == AppState::PLAYING) {
        // dans update(), à la place de gametime :
        distance += chunkManager->getRunspeed() * deltaTime;
        chunkManager->update(deltaTime);
        if (activeScene->playerId != UINT32_MAX) {
            auto& rb = activeScene->rigidbodies[activeScene->playerId];
            constexpr float SPEED = 5.0f;
            rb.velocity.x() = 0.0f;
            rb.velocity.z() = 0.0f;
            if (keyboard.consumeLeft() && lanePosition < Lane::COUNT - 1) { lastLanePosition = lanePosition; this->lanePosition++; }
            if (keyboard.consumeRight() && lanePosition > 0) { lastLanePosition = lanePosition; this->lanePosition--; }
            if (keyboard.isForward()) rb.velocity.z() =  SPEED;
            if (keyboard.isJump() && rb.onGround) {
                rb.velocity.y() = 12.5f; // a ajuster
                if (animManager.getCurrentAnim() != ANIM_JUMP)
                    animManager.setAnimation(ANIM_JUMP);
            }
            if (keyboard.isBack() && !rb.onGround) { // touche bas : sans effet au sol, chute rapide seulement en l'air
                rb.velocity.z() = -SPEED;
                rb.velocity.y() = -20.0f; // a ajuster : ecrase la vitesse Y courante tant que la touche est maintenue
            }
            float targetX = Lane::centerX(lanePosition);
            float currentX = activeScene->transforms[activeScene->playerId].getPosition().x();
            activeScene->transforms[activeScene->playerId].move({(targetX - currentX) * SPEED * deltaTime, 0, 0});
        }

        PhysicsSystem::update(activeScene->transforms, activeScene->rigidbodies, deltaTime);
        CollisionSystem::resolveEntities(activeScene->transforms, activeScene->colliders, activeScene->rigidbodies, activeScene->triggers);

        // onTrigger peut tirer plusieurs fois pour le meme coin sur une frame (une paire de box par overlap) :
        // pickups.count(id) deduplique, l'entite disparait des le premier passage
        for (EntityId id : pendingPickups) {
            if (activeScene->pickups.count(id)) {
                activeScene->destroyEntity(id);
                if (coinCount < std::numeric_limits<unsigned int>::max())
                    coinCount++;
            }
        }
        pendingPickups.clear();

        for (auto& [id, pickup] : activeScene->pickups) {
            (void)pickup;
            activeScene->transforms[id].addRotation(0.0f, COIN_SPIN_SPEED * deltaTime, 0.0f);
        }

        if (activeScene->playerId != UINT32_MAX
            && activeScene->rigidbodies[activeScene->playerId].onGround
            && animManager.getCurrentAnim() != ANIM_RUN)
            animManager.setAnimation(ANIM_RUN, RUN_ANIM_SPEED); // retour au sol : reprend la course

        if (activeScene->playerId != UINT32_MAX)
            camera.follow(activeScene->transforms[activeScene->playerId].getPosition());

        for (auto& [id, render] : activeScene->renders)
            animManager.update(*render.model, deltaTime);
    }
};

void App::processEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		event(e, this->camera, this->running, this->screenW, this->screenH, this->uiRenderer, this->textRenderer);
		mouse.processEvent(e);
		if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F
			&& (e.key.keysym.mod & KMOD_CTRL))
			showFps = !showFps;
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

void App::drawGameWorld(const Matrix<float>& view, Matrix<float>& projection) {
    auto drawEntity = [&](EntityId id, RenderComponent& render, RenderPass pass) {
        LoadedModel& lm = *render.model;
        const Scene& scene = lm.gltf.scenes[lm.gltf.defaultScene];
        Matrix<float> modelMat = activeScene->transforms[id].getModelMatrix() * transform.getModelMatrix();
        for (int rootIdx : scene.rootNodes)
            renderer.renderNode(lm, rootIdx, modelMat, view, projection, render.hiddenNodes, pass, camera, skybox.getIrradianceMapId(), skybox.getPrefilterMapId());
    };

    // 1. opaques + MASK
    renderer.beginOpaquePass();
    for (auto& [id, render] : activeScene->renders)
        drawEntity(id, render, RenderPass::Opaque);

    // 2. skybox : remplit les pixels restants avant les transparents
    skybox.draw(camera.buildViewNoTranslation(), projection);

    // 3. transparents (BLEND) : tries loin -> proche, profondeur gelee
    byDistance.clear();
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
    float endX = textRenderer.drawText(score, "Roboto", 20, 40, 32, fontManager.getFont("Roboto"), 0.03f);
    textRenderer.drawText(" Meters", "CalliCat", endX, 40, 32, fontManager.getFont("CalliCat"), 0.03f);

    float coinEndX = textRenderer.drawText(std::to_string(coinCount), "Roboto", screenW - 160, 40, 32, fontManager.getFont("Roboto"), 0.03f);
    textRenderer.drawText(" Coins", "CalliCat", coinEndX, 40, 32, fontManager.getFont("CalliCat"), 0.03f);
};

void App::render() {
    static bool loggedSamples = false;
    if (!loggedSamples) {
        loggedSamples = true;
        GLint sampleBuffers = 0, samples = 0;
        glGetIntegerv(GL_SAMPLE_BUFFERS, &sampleBuffers);
        glGetIntegerv(GL_SAMPLES, &samples);
        std::cerr << "MSAA reel au rendu : GL_SAMPLE_BUFFERS=" << sampleBuffers << " GL_SAMPLES=" << samples << std::endl;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Matrix<float> view = camera.buildView();
    Matrix<float> projection = camera.buildProjection();
    menuContext.screenW = screenW; // menuContext.screenW/H sont une copie prise a la construction, jamais resynchronisee sinon
    menuContext.screenH = screenH;
    if (state == AppState::GAME_OVER || state == AppState::PAUSED) {
        SDL_ShowCursor(SDL_ENABLE);
        SDL_SetRelativeMouseMode(SDL_FALSE);
        drawGameWorld(view, projection); // monde fige : update() ne tourne plus tant que menus est non-vide

        menuFadeTime += deltaTime;
        float alpha = std::min(menuFadeTime / MENU_FADE_DURATION, 1.0f) * MENU_FADE_MAX_DARKNESS;
        uiRenderer.drawUIComponent(0.0f, 0.0f, 0.0f, (float)screenW, (float)screenH, 0.0f, 0.0f, 0.0f, alpha);

        if (!menus.empty()) {
            menus.top()->update(keyboard, mouse, fontManager, screenW, screenH, menus);
            if (!menus.empty())
                menus.top()->draw(menuContext);
        }
    } else if (!menus.empty()) {
        SDL_ShowCursor(SDL_ENABLE);
        SDL_SetRelativeMouseMode(SDL_FALSE); // libere le curseur (mode relatif = capture + masque, ignore ShowCursor)
        int mX, mY;
        SDL_GetMouseState(&mX, &mY);
        float mouseU = (float)mX / this->screenW;
        float mouseV = (float)mY / this->screenH;
        (void)mouseU;
        (void)mouseV;
        camera.rotateH(deltaTime * 10.0f);
        skybox.draw(camera.buildViewNoTranslation(), projection);

            menus.top()->update(keyboard, mouse, fontManager, screenW, screenH, menus);
        if (!menus.empty())
            menus.top()->draw(menuContext);
    } else if (state == AppState::PLAYING) {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
        drawGameWorld(view, projection);
    }
    if (showFps)
        textRenderer.drawText(std::to_string(fpsDisplay) + " FPS", "Roboto", screenW - 160, 80, 32, fontManager.getFont("Roboto"), 0.01f);
    SDL_GL_SwapWindow(window.getWin());
};

void App::run(){
    SDL_GL_SetSwapInterval(1);
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
	// valeur affichee : moyenne sur 0.5 s, sinon le chiffre vibre trop pour etre lu
	fpsAccumTime += deltaTime;
	fpsAccumFrames++;
	if (fpsAccumTime >= 0.5f) {
		fpsDisplay = (int)(fpsAccumFrames / fpsAccumTime);
		fpsAccumTime = 0.0f;
		fpsAccumFrames = 0;
	}
};

void App::resetGame() {
    keyboard.reset(); // touche relachee pendant que le menu etait ouvert : evite un flag bloque
    this->state = AppState::PLAYING;
    this->lanePosition = Lane::COUNT / 2;
    distance = 0.0f;
    // coinCount n'est PAS remis a 0 : c'est le portefeuille, credite en temps reel a chaque ramassage
    pendingPickups.clear();

    activeScene->rigidbodies[activeScene->playerId].velocity = {0,0,0};
    activeScene->transforms[activeScene->playerId].setPosition(Lane::centerX(lanePosition), 0, 0); //pour virer l'animation de transition


    chunkManager.emplace(*activeScene, modelLoader, Lane::COUNT, 7.35f, 5.0f, "resources/Chunk.gltf", std::vector<std::string>{"resources/Obstacles.gltf", "resources/screens.gltf", "resources/RandomVents.gltf", "resources/coin.gltf"}, [this]() { this->triggerGameOver(); }, [this]() { this->lanePosition = this->lastLanePosition; }, [this](EntityId id) { this->pendingPickups.push_back(id); });
};

void App::triggerGameOver() {
    this->state = AppState::GAME_OVER;
    this->menuFadeTime = 0.0f;
    writeSave(); // checkpoint : les coins de cette run sont deja dans coinCount (credit temps reel)
    menus.push(std::make_unique<GameOverMenu>(
        [this]() { resetGame(); },
        [this]() {
            auto self = std::move(menus.top());
            menus.pop();
            while (!menus.empty())
                menus.pop();
            menus.push(std::make_unique<MainMenu>(
                [this]() { keyboard.reset(); menus.pop(); },
                [this]() { running = false;},
                [this](const std::string& model) { activeScene->setPlayerModel(model, modelLoader); writeSave(); },
                skins, coinCount
            ));
        },
        distance
    ));
};

void App::triggerPause() {
    this->state = AppState::PAUSED;
    this->menuFadeTime = 0.0f;
    menus.push(std::make_unique<PauseMenu>(
        [this]() { keyboard.reset(); this->state = AppState::PLAYING; },
        [this]() { resetGame(); },
        [this]() {
            auto self = std::move(menus.top());
            menus.pop();
            while (!menus.empty())
                menus.pop();
            menus.push(std::make_unique<MainMenu>(
                [this]() { keyboard.reset(); menus.pop(); },
                [this]() { running = false;},
                [this](const std::string& model) { activeScene->setPlayerModel(model, modelLoader); writeSave(); },
                skins, coinCount
            ));
        }
    ));
};

