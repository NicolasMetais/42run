#pragma once
#include <vector>
#include <BufferView.hpp>
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

struct SparseAccessor {
    uint32_t count = 0;
    int indicesAccessor = -1;
    int valuesAccessor = -1;
};

struct AccessorView {
	BufferView* bufferView = nullptr;
	uint32_t count = 0;
	ComponentType component;
	ValueType type;
	bool normalized = false;
	uint32_t byteOffset = 0;
    std::string name;
    SparseAccessor sparse; //pas implementer mais sert a optimiser les gros binaire pour ne modifier que certaines partie
    std::unordered_map<std::string, nlohmann::json> extensions; // pas implementer gestion d'addon/plugin
	std::unordered_map<std::string, nlohmann::json> extras; //flag d'annotation. Parse et stocker mais pas utiliser par le programme intentionnelement
};