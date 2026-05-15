#pragma once
#include <iostream>
#include <vector>
#include <json.hpp>
#include <LoadedModel.hpp>
#include <ModelLoader.hpp>
#include <SceneManager.hpp>

struct GameScene {
	SceneManager sceneManager;
    std::string name;
    std::vector<LoadedModel> models;

    static GameScene fromJson(const std::string& path, ModelLoader& loader);
};