#pragma once
#include <iostream>
#include <GltfModel.hpp>
#include <Scene.hpp>
#include <LoadedModel.hpp>


class SceneManager {
    public:
        int currentSceneIndex = -1;
        const LoadedModel* activeModel = nullptr;

        void setModel(const LoadedModel* model);
        void loadScene(int index);
        const Scene& getScene() const;
        SceneManager() {};
};