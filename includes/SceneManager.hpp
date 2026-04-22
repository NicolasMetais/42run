#pragma once
#include <iostream>
#include <GltfModel.hpp>
#include <Scene.hpp>

class SceneManager {
    public:
        const Scene* currentScene;
        const GltfModel* model;
};