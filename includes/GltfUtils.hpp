#pragma once
#include <cstddef>
#include <cassert>
#include <GltfTypes.hpp>

inline size_t componentSize(ComponentType c) {
    switch (c) {
        case ComponentType::BYTE:
        case ComponentType::UNSIGNED_BYTE: return 1;
        case ComponentType::SHORT:
        case ComponentType::UNSIGNED_SHORT: return 2;
        case ComponentType::UNSIGNED_INT:
        case ComponentType::FLOAT: return 4;
        default: assert(false); return 0;
    }
}

inline uint32_t componentCount(ValueType t) {
    switch (t) {
        case ValueType::SCALAR: return 1;
        case ValueType::VEC2:   return 2;
        case ValueType::VEC3:   return 3;
        case ValueType::VEC4:   return 4;
        case ValueType::MAT2:   return 4;
        case ValueType::MAT3:   return 9;
        case ValueType::MAT4:   return 16;
        default: assert(false); return 0;
    }
}