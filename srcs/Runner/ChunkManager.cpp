#include <Runner/ChunkManager.hpp>

ChunkManager::ChunkManager(GameScene& scene, ModelLoader& loader, int laneNb, float chunkLength, float runSpeed
                , const std::string& floorMesh, const std::vector<std::string>& obstaclesMeshes)
                : generator(floorMesh, obstaclesMeshes, loader, scene), scene(scene), laneNb(laneNb), chunkLength(chunkLength), runSpeed(runSpeed) {
                    for(int i = 0; i < 4; ++i)
                        ChunkQueue.push_back(generator.generateNewChunk(i * chunkLength));
                };

void ChunkManager::destroyChunk() {
    for (auto& ids : ChunkQueue.front().ids)
        scene.destroyEntity(ids);
    ChunkQueue.pop_front();
};

void ChunkManager::update(float deltaTime) {
    for(auto& chunk : ChunkQueue) {
        chunk.zPos -= this->runSpeed * deltaTime;
        for (auto& ids : chunk.ids)
            scene.transforms[ids].move({0,0 , -this->runSpeed * deltaTime});
        }
    if (ChunkQueue.front().zPos <= -this->chunkLength) {
        destroyChunk();
        ChunkQueue.push_back(generator.generateNewChunk(ChunkQueue.back().zPos + this->chunkLength));
    }
};
