#pragma once
#include <iostream>
#include <deque>
#include <vector>
#include <Runner/Chunk.hpp>
#include <Loader/ModelLoader.hpp>
#include <Scene/GameScene.hpp>
#include <Runner/ChunkGenerator.hpp>

class ChunkManager {
    private:
        std::deque<Chunk> ChunkQueue;
        ChunkGenerator generator;
        GameScene& scene;
        int laneNb;
        float chunkLength;
        float runSpeed;
        int chunkCount = 0;
    public:
        ChunkManager(GameScene& scene, ModelLoader& loader, int laneNb, float chunkLength, float runSpeed, const std::string& floorMesh, const std::vector<std::string>& obstaclesMeshes, std::function<void()> func);
        void update(float deltaTime);
        void destroyChunk();
        int getChunkCount() { return this->chunkCount; };
        int getRunspeed() { return this->runSpeed; };
        ~ChunkManager();

};