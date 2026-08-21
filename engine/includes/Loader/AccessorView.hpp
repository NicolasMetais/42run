#pragma once

struct GltfModel;

#include <vector>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <cassert>
#include <algorithm>
#include <Loader/BufferView.hpp>
#include <Loader/Buffer.hpp>
#include <Matrix/Matrix.hpp>
#include <Loader/GltfUtils.hpp>
#include <json.hpp>
#include <cstdint>

/** @brief Sparse storage override for a subset of accessor values. */
struct SparseAccessor {
    uint32_t count = 0;           ///< Number of elements stored sparsely.
    int indicesAccessor = -1;     ///< Accessor index for sparse indices.
    int valuesAccessor = -1;      ///< Accessor index for sparse values.
};

/** @brief View into a GLTF binary buffer with type and layout metadata. */
struct AccessorView {
	BufferView* bufferView = nullptr; ///< Pointer to the associated buffer view.
	uint32_t count = 0;               ///< Number of elements.
	ComponentType component;          ///< Underlying component type (e.g. FLOAT, SHORT).
	ValueType type;                   ///< Element type (e.g. VEC3, MAT4).
	bool normalized = false;          ///< Whether integer values are normalized to [0,1].
	uint32_t byteOffset = 0;          ///< Byte offset relative to the buffer view start.
    std::string name;
    SparseAccessor sparse;
    std::unordered_map<std::string, nlohmann::json> extensions;
	std::unordered_map<std::string, nlohmann::json> extras;

    /** @brief Read a generic float vector of arbitrary component count at element @p index. */
    Vector<float> getVector(size_t index, size_t components, const uint8_t* basePtr) const;
    /** @brief Read a VEC2 at element @p i. */
    Vector<float> getVector2(size_t i,  const uint8_t* basePtr) const;
    /** @brief Read a VEC3 at element @p i. */
    Vector<float> getVector3(size_t i, const uint8_t* basePtr) const;
    /** @brief Read a VEC4 at element @p i. */
    Vector<float> getVector4(size_t i, const uint8_t* basePtr) const;
    /** @brief Read the raw component at @p ptr and return it as a float, handling all component types. */
    float readComponentAsFloat(const uint8_t* ptr) const;

    /**
     * @brief Read a single scalar element of type T at element @p index.
     * @tparam T Target type (float or integer). Uses constexpr branch to avoid runtime overhead.
     * @param index Element index.
     * @param basePtr Pointer to the start of the binary buffer.
     * @return The scalar value cast to T.
     */
    template<typename T>
    T getScalar(size_t index, const uint8_t* basePtr) const {

        size_t stride = bufferView->byteStride != 0 ? bufferView->byteStride : componentSize(component);

        const uint8_t* ptr = basePtr + bufferView->byteOffset + byteOffset + index * stride;

        if constexpr (std::is_same_v<T, float>) {
            return readComponentAsFloat(ptr);
        } else {
            T v{};
            std::memcpy(&v, ptr, sizeof(T));
            return v;
        }
    };
};
