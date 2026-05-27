#include <Runner/ChunkManager.hpp>

ChunkManager::ChunkManager(GameScene& scene, ModelLoader& loader, int laneNb, float chunkLength, float runSpeed
                , const std::string& floorMesh, const std::vector<std::string>& obstaclesMeshes, std::function<void()> func)
                : generator(floorMesh, obstaclesMeshes, loader, scene, chunkLength, func), scene(scene), laneNb(laneNb), chunkLength(chunkLength), runSpeed(runSpeed) {
                ChunkQueue.push_back(generator.generateEmptyChunk(0));  
                for (int i = 1; i < 10; ++i)
                        ChunkQueue.push_back(generator.generateNewChunk((i * chunkLength) * 2));
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
        ChunkQueue.push_back(generator.generateNewChunk(ChunkQueue.back().zPos + chunkLength * 2));
    }
    runSpeed += 0.5f * deltaTime;
};

ChunkManager::~ChunkManager() {
    while (!ChunkQueue.empty())
        destroyChunk();
};
