#include <Runner/ChunkGenerator.hpp>

ChunkGenerator::ChunkGenerator(const std::string& floorMesh, const std::vector<std::string>& obstacleMeshes, ModelLoader& loader, GameScene& scene, float chunkLength, std::function<void()> func) : rng(std::random_device{}()), floorMesh(floorMesh), obstacleMeshes(obstacleMeshes), loader(loader), scene(scene), chunkLength(chunkLength), triggerfunc(func) {};

void ChunkGenerator::screenGenerator(Chunk& newChunk, float x, float y, float z) {
    std::uniform_int_distribution<int> screenbackground(1, 4);
    EntityId id = scene.createEntity();
    newChunk.ids.push_back(id);
    LoadedModel& lm = loader.load(obstacleMeshes[1]);

    scene.transforms[id].setPosition(x, y, z);
    scene.transforms[id].setScale(2.0f);

    RenderComponent screenRender{&lm};
    int bg = screenbackground(rng);
    for (int i = 1; i <= 4; ++i) {
        if (i != bg)
            screenRender.hideNode("BG_" + std::to_string(i));
    }
    scene.renders[id] = screenRender;
};

void ChunkGenerator::ventsGenerator(Chunk& newChunk, float x, float y, float z) {
    std::uniform_int_distribution<int> vent(1, 3);
    EntityId id = scene.createEntity();
    newChunk.ids.push_back(id);
    LoadedModel& lm = loader.load(obstacleMeshes[1]);

    scene.transforms[id].setPosition(x, y, z);
    scene.transforms[id].setScale(2.0f);

    RenderComponent screenRender{&lm};
    int bg = vent(rng);
    for (int i = 1; i <= 3; ++i) {
        if (i != bg)
            screenRender.hideNode("Rvents" + std::to_string(i));
    }
    scene.renders[id] = screenRender;
};

Chunk ChunkGenerator::generateNewChunk(float spawnPos) {

    Chunk newChunk;

    EntityId id = scene.createEntity();
    newChunk.ids.push_back(id);
    LoadedModel& lm = loader.load(floorMesh);
    scene.transforms[id].setPosition(0,0, spawnPos);
    scene.transforms[id].setScale(7.0);
    scene.renders[id] = {&lm};
    scene.colliders[id] = scene.colliderFromModel(lm);
    newChunk.zPos = spawnPos;

    constexpr float DESK_SCALE = 2.0f;
    std::uniform_int_distribution<int> skip(0, 2);
    std::uniform_int_distribution<int> isopen(0, 1);
    std::uniform_int_distribution<int> isScreen(0, 3);
    std::uniform_int_distribution<int> pathSelect(0, 2);
    std::uniform_int_distribution<int> coinflip(0, 1);
    int select = pathSelect(rng);
    (void)select;
    for (int pair = 0; pair < Lane::COUNT / 2; ++pair) {
        if (skip(rng) == 0) continue;
        EntityId id = scene.createEntity();
        newChunk.ids.push_back(id);
        LoadedModel& lm = loader.load(obstacleMeshes[0]);
        RenderComponent render{&lm};
        float x = (Lane::centerX(pair * 2) + Lane::centerX(pair * 2 + 1)) * 0.5f;
        scene.transforms[id].setPosition(x, -0.2, spawnPos);
        scene.transforms[id].setScale(DESK_SCALE);

        int Lvents = skip(rng);
        switch (Lvents) {
            case 0:
                
            case 1:

            default: break;
        }
        int Rvents = skip(rng);

        int screens = isScreen(rng);
        int left = coinflip(rng);
        bool showC1 = true, showC2 = true;
        switch(screens) {
            case 0:
                (left ? showC1 : showC2) = false; [[fallthrough]];
            case 1:
                (left ? showC2 : showC1) = false; break;
            default: break;
        }
        if (!showC1) render.hideNode("Computer 1");
        if (!showC2) render.hideNode("Computer 2");


        constexpr float DESK_Y = -0.2f;
        if (showC1) screenGenerator(newChunk, x + 1.28f * DESK_SCALE, DESK_Y * DESK_SCALE, spawnPos - 0.5f * DESK_SCALE);
        if (showC2) screenGenerator(newChunk, x - 1.24f * DESK_SCALE, DESK_Y * DESK_SCALE, spawnPos - 0.5f * DESK_SCALE);

        if (isopen(rng))
            render.hideNode("Closed");
        scene.renders[id] = render;
        // meme masque pour le collider, sinon mur invisible
        scene.colliders[id] = scene.colliderFromModel(lm, render.hiddenNodes);

        TriggerComponent trig;
        trig.onTrigger = this->triggerfunc;
        scene.triggers[id] = trig;
    }
        
    return newChunk;
};

Chunk ChunkGenerator::generateEmptyChunk(float spawnPos) {
    Chunk newChunk;

    EntityId id = scene.createEntity();
    newChunk.ids.push_back(id);
    LoadedModel& lm = loader.load(floorMesh);
    scene.transforms[id].setPosition(0,0, spawnPos);
    scene.transforms[id].setScale(7.0);
    scene.renders[id] = {&lm};
    scene.colliders[id] = scene.colliderFromModel(lm);
    newChunk.zPos = spawnPos;
    return newChunk;
};




