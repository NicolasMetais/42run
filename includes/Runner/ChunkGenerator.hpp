#pragma once
#include <iostream>
#include <vector>
#include <Loader/ModelLoader.hpp>
#include <GameScene.hpp>
#include <Runner/Chunk.hpp>

class ChunkGenerator {
    private:
        std::string floorMesh;
        std::vector<std::string> obstacleMeshes;
        ModelLoader& loader;
        GameScene& scene;
    public:
        ChunkGenerator(const std::string& floorMesh, const std::vector<std::string>& obstacleMeshes, ModelLoader& loader, GameScene& scene);
        Chunk generateNewChunk(float lastZpos);
};