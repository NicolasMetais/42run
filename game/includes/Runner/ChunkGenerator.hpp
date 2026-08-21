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
        std::function<void()> killFunc;
        std::function<void()> bumpFunc;
        std::function<void(EntityId)> pickupFunc;
        // paire garantie franchissable du dernier chunk genere : la suivante
        // est tiree a +/-1 de celle-ci pour que le chemin reste continu
        int lastFreePair = 1;
        // lanes de la paire libre effectivement praticables (dessous ouvert = les deux ;
        // ecran cache d'un seul cote = une seule) : lu par generateEmptyChunk pour poser les coins
        std::vector<int> lastSafeLanes;
        // derniere lane utilisee pour un trail de coins au sol (-1 = aucune encore) :
        // sert a detecter un changement de ligne entre deux chunks consecutifs
        int lastGroundLane = -1;
        // hauteurs Y provisoires pour les coins, a ajuster une fois testees in-game
        static constexpr float COIN_Y_GROUND = 0.1f; ///< coins au sol / sous le bureau
        static constexpr float COIN_Y_DESK = 3.6f;   ///< coins poses sur le dessus du bureau
        /** @brief Tirage uniforme dans [min, max] inclus, sur le rng du generateur. */
        int roll(int min, int max) { return std::uniform_int_distribution<int>(min, max)(rng); }
        /** @brief Repartit count coins le long de Z (centres sur centerZ) en tournant sur les lanes donnees.
         * Si isGround et qu'un changement de ligne est detecte par rapport au dernier trail au sol,
         * la piece la plus proche du joueur (bord le plus proche de Z=0) est omise. */
        void spawnCoinTrail(Chunk& newChunk, const std::vector<int>& lanes, int count, float centerZ, float y, bool isGround);
    public:
        ChunkGenerator(const std::string& floorMesh, const std::vector<std::string>& obstacleMeshes, ModelLoader& loader, GameScene& scene, float chunkLength, std::function<void()> killFunc, std::function<void()> bumpFunc, std::function<void(EntityId)> pickupFunc);
        Chunk generateNewChunk(float spawnPos);
        void screenGenerator(Chunk& newChunk, float x, float y, float z);
        void ventsGenerator(Chunk& newChunk, float z, bool mirrored);
        void coinGenerator(Chunk& newChunk, float x, float y, float z);
        Chunk generateEmptyChunk(float spawnPos);

};