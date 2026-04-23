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
    std::cout << "  material ptr: " << sm.material << "\n";

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

App::App(int width, int height) : window(width, height), renderer(), mesh()
		, camera(static_cast<float>(width), static_cast<float>(height), Vector<float>{0, 1, 3}, Vector<float>{0,0,0}, Vector<float>{0,1,0}), running(true) {
    this->skybox.generateIrradianceMap();
    this->skybox.generatePrefilterMap();

	// this->transform.setScale(1.0f);
	// Vector<float> cent(3);
	// cent = (data.max + data.min) * 0.5f;
	// this->transform.setPosition(-cent.x(), -cent.y(), -cent.z());
};

App::~App(){};

void App::LoadNewModel(std::string filename) {
    LoadedModel lm;
    lm.gltf.parseJson(filename);
    lm.meshes.reserve(lm.gltf.meshes.size());
    for (auto& mesh : lm.gltf.meshes) {
        lm.meshes.push_back(gltf.buildMeshData(lm.gltf, mesh));
        utils::prepareMats(lm.meshes.back(), textureManager);
        for (auto& sub: lm.meshes.back().submeshes)
            renderer.InitMesh(sub);
    }
    models.push_back(std::move(lm));

    sceneManager.setModel(&models[0]);
    sceneManager.loadScene(models[0].gltf.defaultScene);

    Vector<float> globalMin = {FLT_MAX, FLT_MAX, FLT_MAX};
    Vector<float> globalMax = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
    for (auto& m : models.back().meshes) {
        for (int k = 0; k < 3; k++) {
            if (m.min[k] < globalMin[k]) globalMin[k] = m.min[k];
            if (m.max[k] > globalMax[k]) globalMax[k] = m.max[k];
        }
    }
    Vector<float> center = (globalMin + globalMax) * 0.5f;
    float radius = (globalMax - globalMin).length() * 0.5f;
    camera.fitToScene(center, radius);
};


void App::update() {};

void App::processEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		event(e, this->camera, this->running); //resize window + cross
		mouse.processEvent(e);//mouse events
		keyboard.processEvent(e, this->running, this->camera, this->fps, this->mouselock); //keyboard events
	}
	mouse.applyRotation(this->transform, this->camera); //mouse rotation
	keyboard.applyMovement(this->camera, this->transform, this->deltaTime); //keyboard movement
};

void App::renderNode(LoadedModel& lm, int nodeIdx, const Matrix<float>& parentWorld,
                     const Matrix<float>& view, const Matrix<float>& projection) {
	const Node& node = lm.gltf.nodes[nodeIdx];
	Matrix<float> world = parentWorld * utils::nodeLocalMatrix(node);
	Matrix<float> mvp = projection * view * world;

	if (node.mesh >= 0 && node.mesh < (int)lm.meshes.size())
		renderer.rendering(mvp, lm.meshes[node.mesh], world, camera, skybox.getIrradianceMapId(), skybox.getPrefilterMapId());

	for (int child : node.children)
		renderNode(lm, child, world, view, projection);
};

void App::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix<float> view = camera.buildView();
	Matrix<float> projection = camera.buildProjection();

	for (auto& lm : models) {
		const Scene& scene = sceneManager.getScene();
		Matrix<float> modelMat = transform.getModelMatrix();
		for (int rootIdx : scene.rootNodes)
			renderNode(lm, rootIdx, modelMat, view, projection);
	}

	skybox.draw(camera.buildViewNoTranslation(), projection);
	SDL_GL_SwapWindow(window.getWin());
};

void App::run(){
    SDL_GL_SetSwapInterval(0);

    int frameCount = 0;
    while(this->running)
    {
        this->deltaTime = timer.tick();
        processEvents();
        update();
        render();
        FPScalculator();
        
        frameCount++;
        // if (frameCount >= 1) {
        //     std::cout << "=== Exiting after 1 frame ===" << std::endl;
        //     break;
        // }
    }
    for (auto& lm : models)
        for (auto& mesh : lm.meshes)
            renderer.cleanup(mesh);
};

void App::FPScalculator() {
	this->fps = 1.0f / this->deltaTime;
};