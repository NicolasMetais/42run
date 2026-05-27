#include <Runner/ChunkGenerator.hpp>

ChunkGenerator::ChunkGenerator(const std::string& floorMesh, const std::vector<std::string>& obstacleMeshes, ModelLoader& loader, GameScene& scene, float chunkLength, std::function<void()> func) : rng(std::random_device{}()), floorMesh(floorMesh), obstacleMeshes(obstacleMeshes), loader(loader), scene(scene), chunkLength(chunkLength), triggerfunc(func) {};

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

    std::uniform_int_distribution<int> dist(0,2);
    int obstacleNb = dist(rng);

    std::array<int, 3> lanes = {0, 1, 2};
        std::shuffle(lanes.begin(), lanes.end(), rng);

    for(int i = 0; i < obstacleNb; ++i) {
        EntityId id = scene.createEntity();
        newChunk.ids.push_back(id);
        LoadedModel& lm = loader.load(obstacleMeshes[0]);
        scene.transforms[id].setPosition((lanes[i] - 1) * 4.0f, 1.5, spawnPos + 7.0f);
        scene.transforms[id].setScale(1.5);
        scene.transforms[id].setRotate(0,3,0);
        scene.renders[id] = {&lm};
        scene.colliders[id] = scene.colliderFromModel(lm);
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




