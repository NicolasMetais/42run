#include <Runner/ChunkGenerator.hpp>
#include <algorithm>

ChunkGenerator::ChunkGenerator(const std::string& floorMesh, const std::vector<std::string>& obstacleMeshes, ModelLoader& loader, GameScene& scene, float chunkLength, std::function<void()> func) : rng(std::random_device{}()), floorMesh(floorMesh), obstacleMeshes(obstacleMeshes), loader(loader), scene(scene), chunkLength(chunkLength), triggerfunc(func) {};

void ChunkGenerator::screenGenerator(Chunk& newChunk, float x, float y, float z) {
    EntityId id = scene.createEntity();
    newChunk.ids.push_back(id);
    LoadedModel& lm = loader.load(obstacleMeshes[1]);

    scene.transforms[id].setPosition(x, y, z);
    scene.transforms[id].setScale(2.0f);

    RenderComponent screenRender{&lm};
    int bg = roll(1, 4);
    for (int i = 1; i <= 4; ++i) {
        if (i != bg)
            screenRender.hideNode("BG_" + std::to_string(i));
    }
    scene.renders[id] = screenRender;
};

void ChunkGenerator::ventsGenerator(Chunk& newChunk, float z, bool mirrored) {
    EntityId id = scene.createEntity();
    newChunk.ids.push_back(id);
    LoadedModel& lm = loader.load(obstacleMeshes[2]);

    scene.transforms[id].setPosition(0, 0, z);
    scene.transforms[id].setScale(7.0f);
    // miroir pour le cote gauche — legal uniquement car decoratif :
    // pas de collider, et CollisionSystem ignore la rotation du Transform
    if (mirrored)
        scene.transforms[id].setRotate(0.0f, 3.1415926f, 0.0f);

    RenderComponent ventsRenderer{&lm};
    int bg = roll(1, 3);
    for (int i = 1; i <= 3; ++i) {
        if (i != bg)
            ventsRenderer.hideNode("RVents" + std::to_string(i));
    }
    scene.renders[id] = ventsRenderer;
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

    // chemin garanti : la paire libre derive d'au plus 1 par rapport au chunk
    // precedent (toujours rattrapable), et son passage est force plus bas
    int freePair = std::clamp(lastFreePair + roll(-1, 1), 0, (Lane::COUNT / 2) - 1);
    lastFreePair = freePair;

    // branches de la vent principale : 0 -> deux, 1 -> une, 2 -> aucune
    // deux branches du meme cote = offsets z opposes pour ne pas se superposer
    constexpr float VENT_SPACING = 3.5f;
    int Lvents = roll(0, 2);
    if (Lvents <= 1) ventsGenerator(newChunk, spawnPos - VENT_SPACING, true);
    if (Lvents == 0) ventsGenerator(newChunk, spawnPos + VENT_SPACING, true);
    int Rvents = roll(0, 2);
    if (Rvents <= 1) ventsGenerator(newChunk, spawnPos - VENT_SPACING, false);
    if (Rvents == 0) ventsGenerator(newChunk, spawnPos + VENT_SPACING, false);
    for (int pair = 0; pair < Lane::COUNT / 2; ++pair) {
        // -1 = paire normale ; passage force sur la paire libre :
        // 0 pas de bureau, 1 dessous ouvert, 2 un cote sans ecran
        int pass = (pair == freePair) ? roll(0, 2) : -1;
        if (pass == 0) continue;                      // paire libre : pas de bureau
        if (pass == -1 && roll(0, 2) == 0) continue;  // paire normale : absence aleatoire
        EntityId id = scene.createEntity();
        newChunk.ids.push_back(id);
        LoadedModel& lm = loader.load(obstacleMeshes[0]);
        RenderComponent render{&lm};
        float x = (Lane::centerX(pair * 2) + Lane::centerX(pair * 2 + 1)) * 0.5f;
        scene.transforms[id].setPosition(x, -0.2, spawnPos);
        scene.transforms[id].setScale(DESK_SCALE);

        constexpr float DESK_Y = -0.2f;

        int screens = roll(0, 3);
        if (pass == 2 && screens >= 2) screens = 1;   // paire libre : au moins un cote sautable
        int left = roll(0, 1);
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


        if (showC1) screenGenerator(newChunk, x + 1.28f * DESK_SCALE, DESK_Y * DESK_SCALE, spawnPos - 0.5f * DESK_SCALE);
        if (showC2) screenGenerator(newChunk, x - 1.24f * DESK_SCALE, DESK_Y * DESK_SCALE, spawnPos - 0.5f * DESK_SCALE);

        if (pass == 1 || roll(0, 1))                  // paire libre : dessous force ouvert
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




