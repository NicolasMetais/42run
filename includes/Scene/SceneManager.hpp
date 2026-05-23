#pragma once
#include <iostream>
#include <Loader/GltfModel.hpp>
#include <Loader/Scene.hpp>
#include <Loader/LoadedModel.hpp>

/** @brief Tracks the active model and current scene within that model. */
class SceneManager {
    public:
        int currentSceneIndex = -1;         ///< Index of the active scene (-1 = none).
        const LoadedModel* activeModel = nullptr; ///< Non-owning pointer to the currently displayed model.

        /** @brief Sets the model to draw scenes from. */
        void setModel(const LoadedModel* model);
        /** @brief Switches to the scene at @p index within the active model. */
        void loadScene(int index);
        /** @brief Returns the currently active Scene. */
        const Scene& getScene() const;
        SceneManager() {};
};
