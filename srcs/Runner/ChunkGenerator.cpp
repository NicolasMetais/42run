#include <Runner/ChunkGenerator.hpp>

ChunkGenerator::ChunkGenerator(const std::string& floorMesh, const std::vector<std::string>& obstacleMeshes, ModelLoader& loader, GameScene& scene) : floorMesh(floorMesh), obstacleMeshes(obstacleMeshes), loader(loader), scene(scene) {};

Chunk ChunkGenerator::generateNewChunk(float spawnPos) {

    Chunk newChunk;

    EntityId id = scene.createEntity();
    newChunk.ids.push_back(id);
    std::cout << "ici" << std::endl;
    LoadedModel& lm = loader.load(floorMesh);
    std::cout << "la" << std::endl;
    scene.transforms[id].setPosition(0,0, spawnPos);
    scene.transforms[id].setScale(7.0);
    scene.renders[id] = {&lm};
    scene.colliders[id] = scene.colliderFromModel(lm);
    newChunk.zPos = spawnPos;
    return newChunk;
};



