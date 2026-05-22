#include <Systems/CollisionSystem.hpp>
#include <algorithm>

static Vector<float> toWorld(const Vector<float>& local, const Vector<float>& pos, float scale) {
    return Vector<float>{
        pos.x() + local.x() * scale,
        pos.y() + local.y() * scale,
        pos.z() + local.z() * scale
    };
}

bool CollisionSystem::aabbOverlap(
    const Vector<float>& minA, const Vector<float>& maxA,
    const Vector<float>& minB, const Vector<float>& maxB)
{
    return minA.x() <= maxB.x() && maxA.x() >= minB.x()
        && minA.y() <= maxB.y() && maxA.y() >= minB.y()
        && minA.z() <= maxB.z() && maxA.z() >= minB.z();
}

void CollisionSystem::resolveEntities(
    std::unordered_map<EntityId, Transform>& transforms,
    std::unordered_map<EntityId, ColliderComponent>& colliders,
    std::unordered_map<EntityId, RigidbodyComponent>& rigidbodies,
    std::unordered_map<EntityId, TriggerComponent>& triggers)
{
    for (auto& [dynId, rb] : rigidbodies) {
        auto dynTIt = transforms.find(dynId);
        auto dynCIt = colliders.find(dynId);
        if (dynTIt == transforms.end() || dynCIt == colliders.end()) continue;

        Transform& dynT    = dynTIt->second;
        float       dynS   = dynT.getScale();
        const auto& dynPos = dynT.getPosition();

        for (auto& [statId, statCol] : colliders) {
            if (statId == dynId) continue;
            if (rigidbodies.count(statId)) continue;

            auto statTIt = transforms.find(statId);
            if (statTIt == transforms.end()) continue;

            float       statS   = statTIt->second.getScale();
            const auto& statPos = statTIt->second.getPosition();

            for (auto& dynBox : dynCIt->second.boxes) {
                Vector<float> dMin = toWorld(dynBox.min, dynPos, dynS);
                Vector<float> dMax = toWorld(dynBox.max, dynPos, dynS);

                for (auto& statBox : statCol.boxes) {
                    Vector<float> sMin = toWorld(statBox.min, statPos, statS);
                    Vector<float> sMax = toWorld(statBox.max, statPos, statS);

                    if (!aabbOverlap(dMin, dMax, sMin, sMax)) continue;

                    float ox = std::min(dMax.x(), sMax.x()) - std::max(dMin.x(), sMin.x());
                    float oy = std::min(dMax.y(), sMax.y()) - std::max(dMin.y(), sMin.y());
                    float oz = std::min(dMax.z(), sMax.z()) - std::max(dMin.z(), sMin.z());

                    if (triggers.count(statId)) {
                        triggers[statId].onTrigger();
                        continue;
                    }

                    if (oy <= ox && oy <= oz) {
                        float dynCY  = (dMin.y() + dMax.y()) * 0.5f;
                        float statCY = (sMin.y() + sMax.y()) * 0.5f;
                        if (dynCY > statCY) {
                            dynT.move(Vector<float>{0, oy, 0});
                            if (rb.velocity.y() < 0.0f) rb.velocity.y() = 0.0f;
                            rb.onGround = true;
                        } else {
                            dynT.move(Vector<float>{0, -oy, 0});
                            if (rb.velocity.y() > 0.0f) rb.velocity.y() = 0.0f;
                        }
                    } else if (ox <= oz) {
                        float dir = ((dMin.x() + dMax.x()) > (sMin.x() + sMax.x())) ? 1.0f : -1.0f;
                        dynT.move(Vector<float>{dir * ox, 0, 0});
                        rb.velocity.x() = 0.0f;
                    } else {
                        float dir = ((dMin.z() + dMax.z()) > (sMin.z() + sMax.z())) ? 1.0f : -1.0f;
                        dynT.move(Vector<float>{0, 0, dir * oz});
                        rb.velocity.z() = 0.0f;
                    }
                }
            }
        }
    }
}
