#pragma once
#include <iostream>
#include <deque>
#include <vector>
#include <Runner/Chunk.hpp>
#include <Loader/ModelLoader.hpp>
#include <GameScene.hpp>
#include <Runner/ChunkGenerator.hpp>

class ChunkManager {
    private:
        std::deque<Chunk> ChunkQueue;
        ChunkGenerator generator;
        GameScene& scene;
        int laneNb;
        float chunkLength;
        float runSpeed;
    public:
        ChunkManager(GameScene& scene, ModelLoader& loader, int laneNb, float chunkLength, float runSpeed, const std::string& floorMesh, const std::vector<std::string>& obstaclesMeshes);
        void update(float deltaTime);
        void destroyChunk();
};