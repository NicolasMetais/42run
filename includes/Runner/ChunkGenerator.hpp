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
        // paire garantie franchissable du dernier chunk genere : la suivante
        // est tiree a +/-1 de celle-ci pour que le chemin reste continu
        int lastFreePair = 1;
        /** @brief Tirage uniforme dans [min, max] inclus, sur le rng du generateur. */
        int roll(int min, int max) { return std::uniform_int_distribution<int>(min, max)(rng); }
    public:
        ChunkGenerator(const std::string& floorMesh, const std::vector<std::string>& obstacleMeshes, ModelLoader& loader, GameScene& scene, float chunkLength, std::function<void()> func);
        Chunk generateNewChunk(float spawnPos);
        void screenGenerator(Chunk& newChunk, float x, float y, float z);
        void ventsGenerator(Chunk& newChunk, float z, bool mirrored);
        Chunk generateEmptyChunk(float spawnPos);

};