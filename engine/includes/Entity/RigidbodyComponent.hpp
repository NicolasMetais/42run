#pragma once
#include <Matrix/Vector.hpp>

struct RigidbodyComponent {
    Vector<float> velocity     = {0, 0, 0};
    bool          onGround     = false;
    bool          affectedByGravity = true;
};
