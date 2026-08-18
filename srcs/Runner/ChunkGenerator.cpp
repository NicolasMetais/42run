#include <Runner/ChunkGenerator.hpp>
#include <algorithm>

ChunkGenerator::ChunkGenerator(const std::string& floorMesh, const std::vector<std::string>& obstacleMeshes, ModelLoader& loader, GameScene& scene, float chunkLength, std::function<void()> killFunc, std::function<void()> bumpFunc) : rng(std::random_device{}()), floorMesh(floorMesh), obstacleMeshes(obstacleMeshes), loader(loader), scene(scene), chunkLength(chunkLength), killFunc(killFunc), bumpFunc(bumpFunc) {};

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

void ChunkGenerator::coinGenerator(Chunk& newChunk, float x, float y, float z) {
    EntityId id = scene.createEntity();
    newChunk.ids.push_back(id);
    LoadedModel& lm = loader.load(obstacleMeshes[3]);

    scene.transforms[id].setPosition(x, y, z);
    scene.transforms[id].setScale(1.5f); // a ajuster
    scene.transforms[id].setRotate(1.5707963f, 0.0f, 0.0f); // redresse sur la tranche : les faces plates (local Z) passent de face au monde (world Y) a face au joueur (world Z)

    RenderComponent coinRender{&lm};
    scene.renders[id] = coinRender;
};

void ChunkGenerator::spawnCoinTrail(Chunk& newChunk, const std::vector<int>& lanes, int count, float centerZ, float y, bool isGround) {
    if (lanes.empty() || count <= 0) return;
    int lane = lanes[roll(0, (int)lanes.size() - 1)]; // une seule lane tiree au hasard parmi celles fournies

    int start = 0;
    if (isGround) {
        if (lastGroundLane != -1 && lastGroundLane != lane && count > 1)
            start = 1; // changement de ligne detecte : une piece en moins pres du joueur
        lastGroundLane = lane;
    }

    float span = chunkLength * 2.0f; // couvre tout le chunk (les chunks sont espaces de chunkLength*2) pour ne pas laisser de trou entre deux chunks consecutifs
    for (int i = start; i < count; ++i) {
        float t = (count == 1) ? 0.5f : (float)i / (count - 1);
        float z = centerZ - span * 0.5f + span * t;
        coinGenerator(newChunk, Lane::centerX(lane), y, z);
    }
};

Chunk ChunkGenerator::generateNewChunk(float spawnPos) {

    Chunk newChunk;

    LoadedModel& lm = loader.load(floorMesh);
    Transform floorT; floorT.setPosition(0, 0, spawnPos); floorT.setScale(7.0);
    for (EntityId id : scene.spawnEntity(lm, floorT, RenderComponent{&lm}))
        newChunk.ids.push_back(id);
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
        // 1 dessous ouvert (1/2), 2 un cote sans ecran (1/2) — toujours un bureau, plus de "pas de bureau"
        int pass = (pair == freePair) ? (roll(0, 1) == 0 ? 1 : 2) : -1;
        LoadedModel& deskLm = loader.load(obstacleMeshes[0]);
        RenderComponent render{&deskLm};
        float x = (Lane::centerX(pair * 2) + Lane::centerX(pair * 2 + 1)) * 0.5f;
        Transform deskT; deskT.setPosition(x, -0.2, spawnPos); deskT.setScale(DESK_SCALE);

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

        bool underneathOpen = (pass == 1 || roll(0, 2) == 0);  // paire libre : dessous force ouvert ; sinon ouvert 1 fois sur 3
        if (underneathOpen) {
            render.hideNode("Closed");
            render.hideNode("OBSTACLE_killClosed");          // le panneau disparait, sa zone de mort aussi
        }

        if (pair == freePair) {
            if (underneathOpen) {
                lastSafeLanes = { freePair * 2, freePair * 2 + 1 }; // dessous ouvert : les deux lanes praticables
                spawnCoinTrail(newChunk, lastSafeLanes, 5, spawnPos, COIN_Y_GROUND, true);
            } else {
                lastSafeLanes.clear();
                if (!showC1) lastSafeLanes.push_back(freePair * 2 + 1); // cote +X (Computer 1) degage
                if (!showC2) lastSafeLanes.push_back(freePair * 2);     // cote -X (Computer 2) degage
                spawnCoinTrail(newChunk, lastSafeLanes, 1, spawnPos, COIN_Y_DESK, false);
            }
        }
        // KILL/bump : entites dediees si le modele porte des nodes OBSTACLE_KILL/OBSTACLE_bump,
        // sinon spawnEntity ne cree que l'entite solide (pas de branchement ici)
        for (EntityId id : scene.spawnEntity(deskLm, deskT, render,
                {{"kill", killFunc}, {"bump", bumpFunc}}))
            newChunk.ids.push_back(id);
    }
        
    return newChunk;
};

Chunk ChunkGenerator::generateEmptyChunk(float spawnPos) {
    Chunk newChunk;

    LoadedModel& lm = loader.load(floorMesh);
    Transform floorT; floorT.setPosition(0, 0, spawnPos); floorT.setScale(7.0);
    for (EntityId id : scene.spawnEntity(lm, floorT, RenderComponent{&lm}))
        newChunk.ids.push_back(id);
    newChunk.zPos = spawnPos;

    spawnCoinTrail(newChunk, lastSafeLanes, 5, spawnPos, COIN_Y_GROUND, true); // chemin vers le dernier passage libre detecte

    return newChunk;
};




