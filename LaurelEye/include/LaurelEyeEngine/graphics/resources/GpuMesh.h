
#pragma once

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/VertexArray.h"

#include <cstdint>

namespace LaurelEye::Graphics {

    enum class IndexType : uint8_t {
        Uint16,
        Uint32
    };

    struct GpuMesh {
        VertexArrayHandle vao{VertexArray::InvalidVertexArray};
        DataBufferHandle vertexBuffer{DataBuffer::InvalidDataBuffer};
        DataBufferHandle indexBuffer{DataBuffer::InvalidDataBuffer};

        uint32_t indexCount{0};
        IndexType indexType{IndexType::Uint32};
    };

} // namespace LaurelEye::Graphics
