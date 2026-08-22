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
        bool spawnEmpty = false; // type du prochain chunk a generer (alterne obstacle/vide)
    public:
        ChunkManager(GameScene& scene, ModelLoader& loader, int laneNb, float chunkLength, float runSpeed, const std::string& floorMesh, const std::vector<std::string>& obstaclesMeshes, std::function<void()> killFunc, std::function<void()> bumpFunc, std::function<void(EntityId)> pickupFunc);
        void update(float deltaTime);
        void destroyChunk();
        int getChunkCount() { return this->chunkCount; };
        int getRunspeed() { return this->runSpeed; };
        ~ChunkManager();

};