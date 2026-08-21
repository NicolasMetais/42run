#include <Scene/SceneManager.hpp>

void SceneManager::setModel(const LoadedModel* model) {
    this->activeModel = model;
};


void SceneManager::loadScene(int index) {
    if (index < 0 || index >= (int)activeModel->gltf.scenes.size())
        throw std::runtime_error("Invalid Scene id");
    this->currentSceneIndex = index;
}; 

const Scene& SceneManager::getScene() const {
    return activeModel->gltf.scenes[this->currentSceneIndex];
};

