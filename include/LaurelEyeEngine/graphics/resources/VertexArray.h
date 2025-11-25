
#pragma once

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"

#include <cstdint>
#include <vector>

namespace LaurelEye::Graphics {

    using VertexArrayHandle = uint32_t;

    namespace VertexArray {
        inline constexpr VertexArrayHandle InvalidVertexArray = UINT32_MAX;
    }

    inline bool isValidVertexArray(VertexArrayHandle h) noexcept {
        return h != VertexArray::InvalidVertexArray;
    }

    /// @brief Supported generic vertex attribute formats.
    enum class VertexAttribFormat : uint8_t {
        Float32,
        Float32x2,
        Float32x3,
        Float32x4,
        Uint8x4Norm,
        // Extend as needed (e.g. Uint16x2, etc.)
    };

    /// @brief One vertex buffer bound to a specific binding index.
    struct VertexBufferBindingDesc {
        DataBufferHandle buffer; ///< Must be DataBufferType::Vertex
        uint32_t bindingIndex;   ///< glVertexArrayVertexBuffer binding slot
        uint32_t stride;         ///< Size of one vertex in bytes
        uint32_t offset;         ///< Byte offset to first vertex in buffer (usually 0)
    };

    /// @brief Description of one vertex attribute (position, normal, UV, etc).
    struct VertexAttributeDesc {
        uint32_t location;       ///< Shader location (layout(location = X))
        uint32_t bindingIndex;   ///< Which vertex stream this attribute reads from
        uint32_t relativeOffset; ///< Offset within one vertex element (struct) in bytes
        VertexAttribFormat format;
    };

    /// @brief Description of a VAO at creation time.
    struct VertexArrayDesc {
        std::vector<VertexBufferBindingDesc> vertexBindings;
        std::vector<VertexAttributeDesc>    attributes;
        DataBufferHandle indexBuffer{ DataBuffer::InvalidDataBuffer }; ///< optional
    };

} // namespace LaurelEye::Graphics
