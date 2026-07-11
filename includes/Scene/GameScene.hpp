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

    EntityId createEntity() { return nextId++; }
    void destroyEntity(EntityId id);

    ColliderComponent colliderFromModel(const LoadedModel& lm, const std::unordered_set<int>& hiddenNodes = {});
void collectBoxes(const GltfModel& gltf, const std::vector<MeshData>& meshes, int nodeIdx, const Matrix<float>& parentWorld, ColliderComponent& col, const std::unordered_set<int>& hiddenNodes);

    static GameScene fromJson(const std::string& path, ModelLoader& loader);
    void loadPlayer(const std::string& path, ModelLoader& loader);
};