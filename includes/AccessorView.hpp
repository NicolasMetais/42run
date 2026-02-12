#pragma once
#include <vector>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <cassert>
#include <algorithm>
#include <BufferView.hpp>
#include <Buffer.hpp>
#include <Matrix/Matrix.hpp>

#include <GltfUtils.hpp>
#include <json.hpp>
#include <cstdint>

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

    Vector<float> getVector(size_t index, size_t components) const;
    Vector<float> getVector2(size_t i) const;
    Vector<float> getVector3(size_t i) const;
    Vector<float> getVector4(size_t i) const;
    float readComponentAsFloat(const uint8_t* ptr) const;

    template<typename T>
    T getScalar(size_t index) const {

        size_t stride = bufferView->byteStride != 0 ? bufferView->byteStride : componentSize(component);


        const uint8_t* ptr = bufferView->base + byteOffset + index * stride;

        if constexpr (std::is_same_v<T, float>) { //constexpr = le if est gere a la compilation et supprime le code qui ne va pas etre use
            return readComponentAsFloat(ptr);
        } else {
            T v{};
            std::memcpy(&v, ptr, sizeof(T));
            return v;
        }
    };
};

