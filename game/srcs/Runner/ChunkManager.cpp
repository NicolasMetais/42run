#include <Runner/ChunkManager.hpp>

ChunkManager::ChunkManager(GameScene& scene, ModelLoader& loader, int laneNb, float chunkLength, float runSpeed
                , const std::string& floorMesh, const std::vector<std::string>& obstaclesMeshes, std::function<void()> killFunc, std::function<void()> bumpFunc, std::function<void(EntityId)> pickupFunc)
                : generator(floorMesh, obstaclesMeshes, loader, scene, chunkLength, killFunc, bumpFunc, pickupFunc), scene(scene), laneNb(laneNb), chunkLength(chunkLength), runSpeed(runSpeed) {
                spawnEmpty = false; // apres les 2 chunks vides forces, le suivant est un chunk d'obstacles, puis ca alterne
                for (int i = 0; i < 10; ++i) {
                        float pos = (i * chunkLength) * 2;
                        bool empty = (i < 2) ? true : spawnEmpty; // les 2 premiers chunks toujours vides : pas de mort au spawn
                        ChunkQueue.push_back(empty ? generator.generateEmptyChunk(pos) : generator.generateNewChunk(pos));
                        if (i >= 2) spawnEmpty = !spawnEmpty;
                }
};

void ChunkManager::destroyChunk() {
    for (auto& ids : ChunkQueue.front().ids) {
        scene.destroyEntity(ids);
        chunkCount++;
    };
    ChunkQueue.pop_front();
};

void ChunkManager::update(float deltaTime) {
    for(auto& chunk : ChunkQueue) {
        chunk.zPos -= this->runSpeed * deltaTime;
        for (auto& ids : chunk.ids)
            scene.transforms[ids].move({0,0 , -this->runSpeed * deltaTime});
        }
    if (ChunkQueue.front().zPos <= -this->chunkLength * 3) {
        destroyChunk();
        float pos = ChunkQueue.back().zPos + chunkLength * 2;
        ChunkQueue.push_back(spawnEmpty ? generator.generateEmptyChunk(pos) : generator.generateNewChunk(pos));
        spawnEmpty = !spawnEmpty;
    }
    runSpeed += 0.5f * deltaTime;
};

ChunkManager::~ChunkManager() {
    while (!ChunkQueue.empty())
        destroyChunk();
};
