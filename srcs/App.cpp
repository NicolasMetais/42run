#include <App.hpp>

// Affichage d'un Vector<float> (supposé en 3D)
void printVector(const Vector<float>& v, const std::string& name)
{
	if (v.size() == 3) {
		std::cout << name << ": ("
				<< v.x() << ", "
				<< v.y() << ", "
				<< v.z() << ")\n";
	}
	else if (v.size() == 2) {
		std::cout << name << ": ("
				<< v.x() << ", "
				<< v.y() << ")\n";
	} else if (v.size() == 4) {
		std::cout << name << ": ("
				<< v.x() << ", "
				<< v.y() << ", "
				<< v.z() <<  ", "
                << v.w() << ")\n";
	}
}

// Affichage d'un Vertex
void printVertex(const Vertex& v)
{
    std::cout << "Vertex:\n";

    printVector(v.position, "  position");
    printVector(v.normal, "  normal");
    std::cout << "tangent.size() = " << v.tangent.size() << std::endl;
    printVector(v.tangent, "  tangent");

    // UVs
    for (size_t i = 0; i < v.uv.size(); ++i)
    {
        printVector(v.uv[i], "  uv[" + std::to_string(i) + "]");
    }

    // Couleurs
    std::cout << "  color: ";
    for (float c : v.color)
    {
        std::cout << c << " ";
    }
    std::cout << "\n";

    // Joints
    std::cout << "  joints: ";
    for (int j : v.joints)
    {
        std::cout << j << " ";
    }
    std::cout << "\n";

    // Weights
    std::cout << "  weights: ";
    for (float w : v.weights)
    {
        std::cout << w << " ";
    }
    std::cout << "\n";
}


// Affichage d'un SubMesh
void printSubMesh(const SubMesh& sm)
{
    std::cout << "SubMesh:\n";
    std::cout << "  VAO: " << sm.VAO << "\n";
    std::cout << "  VBO: " << sm.VBO << "\n";
    std::cout << "  EBO: " << sm.EBO << "\n";
    std::cout << "  material ptr: " << sm.materialIndex << "\n";

    std::cout << "  Vertices count: " << sm.vertices.size() << "\n";
    for (size_t i = 0; i < sm.vertices.size(); ++i)
    {
        std::cout << " Vertex[" << i << "]\n";
        printVertex(sm.vertices[i]);
    }

    std::cout << "  Indices: ";
    for (size_t i = 0; i < sm.indices.size(); ++i)
    {
        std::cout << sm.indices[i] << " ";
    }
    std::cout << "\n";
}

// Affichage d'un MeshData
void printMeshData(const MeshData& mesh)
{
    std::cout << "MeshData:\n";

    printVector(mesh.min, "  min");
    printVector(mesh.max, "  max");
    printVector(mesh.center, "  center");

    std::cout << "  radius: " << mesh.radius << "\n";

    std::cout << "SubMeshes count: " << mesh.submeshes.size() << "\n";
    for (size_t i = 0; i < mesh.submeshes.size(); ++i)
    {
        std::cout << "\nSubMesh[" << i << "]\n";
        printSubMesh(mesh.submeshes[i]);
    }
}

App::App(int width, int height) 
        : window(width, height), renderer(), mesh(), camera(static_cast<float>(width), static_cast<float>(height)
        , Vector<float>{0, 1, 3}, Vector<float>{0,0,0}, Vector<float>{0,1,0}), running(true)
        , modelLoader(renderer, textureManager){
    this->skybox.generateIrradianceMap();
    this->skybox.generatePrefilterMap();

    scenes.push_back(GameScene::fromJson("resources/levels/level1.json", modelLoader));
    activeScene = &scenes[0];
    chunkManager.emplace(*activeScene, modelLoader, 3, 7.0f, 5.0f, "resources/TwoSidedPlane.gltf", std::vector<std::string>{"resources/DamagedHelmet.gltf"}, [this]() { this->running = false; });
    activeScene->loadPlayer("resources/player.json", modelLoader);
	// this->transform.setScale(1.0f);
	// Vector<float> cent(3);
	// cent = (data.max + data.min) * 0.5f;
	// this->transform.setPosition(-cent.x(), -cent.y(), -cent.z());
};

App::~App(){};

void App::update() {
    chunkManager->update(deltaTime);
    if (activeScene->playerId != UINT32_MAX) {
        auto& rb = activeScene->rigidbodies[activeScene->playerId];
        constexpr float SPEED = 5.0f;
        rb.velocity.x() = 0.0f;
        rb.velocity.z() = 0.0f;
        if (keyboard.consumeLeft() && lanePosition < 2 ) this->lanePosition++;
        if (keyboard.consumeRight() && lanePosition > 0) this->lanePosition--;
        if (keyboard.isForward()) rb.velocity.z() =  SPEED;
        if (keyboard.isBack())    rb.velocity.z() = -SPEED;
        if (keyboard.isJump() && rb.onGround) rb.velocity.y() = 8.0f;
        float targetX = (lanePosition - 1) * 4.0f;
        float currentX = activeScene->transforms[activeScene->playerId].getPosition().x();
        activeScene->transforms[activeScene->playerId].move({(targetX - currentX) * SPEED * deltaTime, 0, 0});
    }

    PhysicsSystem::update(activeScene->transforms, activeScene->rigidbodies, deltaTime);
    CollisionSystem::resolveEntities(activeScene->transforms, activeScene->colliders, activeScene->rigidbodies, activeScene->triggers);

    if (activeScene->playerId != UINT32_MAX)
        camera.follow(activeScene->transforms[activeScene->playerId].getPosition());

    for (auto& [id, render] : activeScene->renders)
        animManager.update(*render.model, deltaTime);
};

void App::processEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		event(e, this->camera, this->running);
		mouse.processEvent(e);
		keyboard.processEvent(e, this->running, this->camera, this->fps, this->mouselock);
		// if (e.type == SDL_KEYDOWN) {
		// 	SDL_Keycode sym = e.key.keysym.sym;
		// 	SDL_Scancode sc  = e.key.keysym.scancode;
		// 	if (sc == SDL_SCANCODE_1 || sym == SDLK_1 || sym == SDLK_KP_1) animManager.setAnimation(0);
		// 	if (sc == SDL_SCANCODE_2 || sym == SDLK_2 || sym == SDLK_KP_2 || sc == SDL_SCANCODE_E) animManager.setAnimation(1);
		// 	if (sc == SDL_SCANCODE_3 || sym == SDLK_3 || sym == SDLK_KP_3) animManager.setAnimation(2);
		// }
	}
	mouse.applyRotation(this->transform, this->camera); //mouse rotation
	keyboard.applyMovement(this->camera, this->transform, this->deltaTime); //keyboard movement
};

void App::renderNode(LoadedModel& lm, int nodeIdx, const Matrix<float>& parentWorld,
                     const Matrix<float>& view, const Matrix<float>& projection) {
	const Node& node = lm.gltf.nodes[nodeIdx];
	Matrix<float> world = parentWorld * utils::nodeLocalMatrix(node);
	Matrix<float> mvp = projection * view * world;

	if (node.mesh >= 0 && node.mesh < (int)lm.meshes.size()) {
		const std::vector<Matrix<float>>* jointMats = nullptr;
		if (node.skin >= 0 && node.skin < (int)lm.jointMatrices.size())
			jointMats = &lm.jointMatrices[node.skin];
		renderer.rendering(mvp, lm.meshes[node.mesh], world, camera, skybox.getIrradianceMapId(), skybox.getPrefilterMapId(), jointMats);
	}

	for (int child : node.children)
		renderNode(lm, child, world, view, projection);
};

void App::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix<float> view = camera.buildView();
	Matrix<float> projection = camera.buildProjection();

	for (auto& [id, render] : activeScene->renders) {
		LoadedModel& lm = *render.model;
		const Scene& scene = lm.gltf.scenes[lm.gltf.defaultScene];
		Matrix<float> modelMat = activeScene->transforms[id].getModelMatrix() * transform.getModelMatrix();
		for (int rootIdx : scene.rootNodes)
			renderNode(lm, rootIdx, modelMat, view, projection);
	}

	skybox.draw(camera.buildViewNoTranslation(), projection);
	SDL_GL_SwapWindow(window.getWin());
};

void App::run(){
    SDL_GL_SetSwapInterval(0);
    timer.tick(); // discard loading time

    int frameCount = 0;
    while(this->running)
    {
        this->deltaTime = timer.tick();
        if (this->deltaTime > 0.1f) this->deltaTime = 0.1f; // clamp: évite les sauts physiques si freeze
        processEvents();
        update();
        render();
        FPScalculator();
        
        frameCount++;
    }
    for (auto& [id, render] : activeScene->renders)
        for (auto& mesh : render.model->meshes)
            renderer.cleanup(mesh);
};

void App::FPScalculator() {
	this->fps = 1.0f / this->deltaTime;
};