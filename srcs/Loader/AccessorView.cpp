#include <Loader/AccessorView.hpp>
#include <Loader/GltfModel.hpp>

Vector<float> AccessorView::getVector(size_t index, size_t components, const uint8_t* basePtr) const {
    size_t stride = bufferView->byteStride;
    // size_t maxOffset = bufferView->byteOffset + bufferView->byteLength;
    // size_t actualOffset = bufferView->byteOffset + byteOffset + index * stride;

    // if (actualOffset + (size_t)component * componentSize(component) <= maxOffset)
    //     throw std::runtime_error("vector data corrupted");
    if (stride == 0)
        stride = componentCount(type) * componentSize(component);

    const uint8_t* ptr = basePtr + bufferView->byteOffset + byteOffset + index * stride;
    
    Vector<float> v(components);
    for (size_t i = 0; i < components; ++i)
        v[i] = readComponentAsFloat(ptr + i * componentSize(component));
    return v;
}

Vector<float> AccessorView::getVector2(size_t i, const uint8_t* basePtr) const {
    assert(type == ValueType::VEC2);
        return getVector(i, 2, basePtr);
    }

Vector<float> AccessorView::getVector3(size_t i, const uint8_t* basePtr) const {
    assert(type == ValueType::VEC3);
        return getVector(i, 3, basePtr);
}

Vector<float>AccessorView:: getVector4(size_t i, const uint8_t* basePtr) const {
    assert(type == ValueType::VEC4);
        return getVector(i, 4, basePtr);
}

float AccessorView::readComponentAsFloat(const uint8_t* ptr) const {
    switch (component)
    {
        case ComponentType::FLOAT: {
            float v;
            std::memcpy(&v, ptr, sizeof(v));
            return v;
        }
        case ComponentType::UNSIGNED_BYTE:
            return normalized ? (*ptr / 255.0f) : *ptr;
        case ComponentType::BYTE: {
            int8_t v;
            std::memcpy(&v, ptr, sizeof(v));
            return normalized ? (std::max(-1.0f, v / 127.0f)) : v;
        }
        case ComponentType::UNSIGNED_SHORT: {
            uint16_t v;
            std::memcpy(&v, ptr, sizeof(v));
            return normalized ? ( v / 65535.0f) : v;
        }
        case ComponentType::SHORT: {
            int16_t v;
            std::memcpy(&v, ptr, sizeof(v));
            return normalized ? (std::max(-1.0f, v / 32767.0f)) : v;
        }
        
        default:
            assert(false && "Unsuported component type");
            return 0.0f;
    }
}