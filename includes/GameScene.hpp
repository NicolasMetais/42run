#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <json.hpp>
#include <LoadedModel.hpp>
#include <ModelLoader.hpp>
#include <Transform.hpp>
#include <Entity/ColliderComponent.hpp>
#include <Entity/RenderComponent.hpp>
#include <Entity/RigidbodyComponent.hpp>
#include <ECS.hpp>
#include <SceneManager.hpp>

struct GameScene {
	SceneManager sceneManager;
    std::string name;
    EntityId nextId  = 0;
    EntityId playerId = UINT32_MAX;

    std::unordered_map<EntityId, Transform> transforms;
    std::unordered_map<EntityId, RenderComponent> renders;
    std::unordered_map<EntityId, ColliderComponent> colliders;
    std::unordered_map<EntityId, RigidbodyComponent> rigidbodies;

    EntityId createEntity() { return nextId++; }

    static GameScene fromJson(const std::string& path, ModelLoader& loader);
    void loadPlayer(const std::string& path, ModelLoader& loader);
};