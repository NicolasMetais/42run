#include <Systems/PhysicsSystem.hpp>

void PhysicsSystem::update(
    std::unordered_map<EntityId, Transform>& transforms,
    std::unordered_map<EntityId, RigidbodyComponent>& rigidbodies,
    float dt)
{
    for (auto& [id, rb] : rigidbodies) {
        auto it = transforms.find(id);
        if (it == transforms.end()) continue;
        Transform& t = it->second;

        rb.onGround = false;

        if (rb.affectedByGravity)
            rb.velocity.y() += GRAVITY * dt;

        t.move(rb.velocity * dt);
    }
}
