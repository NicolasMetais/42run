#pragma once
#include <cstdint>

enum class ComponentType : uint16_t {
    BYTE           = 5120,
    UNSIGNED_BYTE  = 5121,
    SHORT          = 5122,
    UNSIGNED_SHORT = 5123,
    UNSIGNED_INT   = 5125,
    FLOAT          = 5126
};

enum class ValueType : uint8_t {
    SCALAR,
    VEC2,
    VEC3,
    VEC4,
    MAT2,
    MAT3,
    MAT4
};