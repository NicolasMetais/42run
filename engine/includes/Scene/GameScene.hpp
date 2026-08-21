#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <json.hpp>
#include <Loader/LoadedModel.hpp>
#include <Loader/ModelLoader.hpp>
#include <Scene/Transform.hpp>
#include <Entity/ColliderComponent.hpp>
#include <Entity/RenderComponent.hpp>
#include <Entity/RigidbodyComponent.hpp>
#include <Entity/TriggerComponent.hpp>
#include <Entity/PickupComponent.hpp>
#include <ECS.hpp>
#include <Scene/SceneManager.hpp>

struct GameScene {
	SceneManager sceneManager;
    std::string name;
    EntityId nextId  = 0;
    EntityId playerId = UINT32_MAX;

    std::unordered_map<EntityId, Transform> transforms;
    std::unordered_map<EntityId, RenderComponent> renders;
    std::unordered_map<EntityId, ColliderComponent> colliders;
    std::unordered_map<EntityId, RigidbodyComponent> rigidbodies;
    std::unordered_map<EntityId, TriggerComponent> triggers;
    std::unordered_map<EntityId, PickupComponent> pickups; ///< entites ramassables (coins), pour la rotation et le parcours dedie

    EntityId createEntity() { return nextId++; }
    void destroyEntity(EntityId id);

    ColliderComponent colliderFromModel(const LoadedModel& lm, const std::unordered_set<int>& hiddenNodes = {});
    void collectBoxes(const GltfModel& gltf, const std::vector<MeshData>& meshes, int nodeIdx, const Matrix<float>& parentWorld, ColliderComponent& col, const std::unordered_set<int>& hiddenNodes);

    /**
     * @brief Spawn unique pour tout objet du jeu : construit toujours une entite
     * solide (le collider habituel, moins les nodes "OBSTACLE_*"), et pour chaque
     * entree de obstacleCallbacks, spawn en plus une entite trigger dediee si un
     * node "OBSTACLE_<cle>" existe dans le modele. L'appelant n'a jamais besoin de
     * savoir si le modele contient des obstacles ou non.
     * @return les ids crees (entite solide en premier), a pousser dans Chunk::ids.
     */
    std::vector<EntityId> spawnEntity(LoadedModel& lm, Transform t, RenderComponent render,
                                       const std::unordered_map<std::string, std::function<void()>>& obstacleCallbacks = {});

    static GameScene fromJson(const std::string& path, ModelLoader& loader);
    void loadPlayer(const std::string& path, ModelLoader& loader);
    /** @brief Change le modele/skin du joueur deja charge (garde position/scale/rigidbody/trigger). */
    void setPlayerModel(const std::string& modelFile, ModelLoader& loader);
};