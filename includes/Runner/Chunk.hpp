#pragma once
#include <iostream>
#include <vector>
#include <ECS.hpp>


struct Chunk {
    std::vector<EntityId> ids;
    float zPos;
};