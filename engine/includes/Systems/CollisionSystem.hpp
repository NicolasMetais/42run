#pragma once
#include <unordered_map>
#include <ECS.hpp>
#include <Scene/Transform.hpp>
#include <Entity/ColliderComponent.hpp>
#include <Entity/RigidbodyComponent.hpp>
#include <Entity/TriggerComponent.hpp>


namespace CollisionSystem {
    // Resolves dynamic (rigidbody) entities against static (collider-only) entities.
    void resolveEntities(
        std::unordered_map<EntityId, Transform>& transforms,
        std::unordered_map<EntityId, ColliderComponent>& colliders,
        std::unordered_map<EntityId, RigidbodyComponent>& rigidbodies,
        std::unordered_map<EntityId, TriggerComponent>& triggers
    );

    // Returns true if two world-space AABBs overlap.
    bool aabbOverlap(
        const Vector<float>& minA, const Vector<float>& maxA,
        const Vector<float>& minB, const Vector<float>& maxB
    );
}
