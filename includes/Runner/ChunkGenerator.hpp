#pragma once
#include <iostream>
#include <vector>
#include <Loader/ModelLoader.hpp>
#include <Scene/GameScene.hpp>
#include <Runner/Chunk.hpp>
#include <random>

class ChunkGenerator {
    private:
        std::mt19937 rng;
        std::string floorMesh;
        std::vector<std::string> obstacleMeshes;
        ModelLoader& loader;
        GameScene& scene;
        float chunkLength;
        std::function<void()> triggerfunc;
    public:
        ChunkGenerator(const std::string& floorMesh, const std::vector<std::string>& obstacleMeshes, ModelLoader& loader, GameScene& scene, float chunkLength, std::function<void()> func);
        Chunk generateNewChunk(float spawnPos);
        void screenGenerator(Chunk& newChunk, float x, float y, float z);
        void ventsGenerator(Chunk& newChunk, float x, float y, float z);
        Chunk generateEmptyChunk(float spawnPos);

};