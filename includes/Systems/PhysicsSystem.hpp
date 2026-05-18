#pragma once
#include <unordered_map>
#include <ECS.hpp>
#include <Transform.hpp>
#include <Entity/RigidbodyComponent.hpp>

namespace PhysicsSystem {
    constexpr float GRAVITY = -9.81f;

    void update(
        std::unordered_map<EntityId, Transform>& transforms,
        std::unordered_map<EntityId, RigidbodyComponent>& rigidbodies,
        float dt
    );
}
