/// @file   LGLDataBufferFactory.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-08-2025
/// @brief Data Buffer Factory for OpenGL.
///
#pragma once

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"

#include <glad/glad.h>

#include <cstdint>
#include <unordered_map>

namespace LaurelEye::Graphics {

    /// @class LGLDataBufferRecord
    /// @brief Record of Data Buffer's GPU and operational info.
    struct LGLDataBufferRecord {
        DataBufferDesc desc{};

        GLuint id = 0;

        // Additional for ring state (SSBO persistent)
        // Mapped memory for persistent buffers:
        // void* mapped = nullptr;
        // ...
    };

    /// @class LGLDataBufferFactory
    /// @brief Data Buffer Factory responsible for all UBO and SSBO GPU
    /// operations and lifecycle.
    class LGLDataBufferFactory {
    public:
        /// \copydoc IRenderDevice::createDataBuffer
        [[nodiscard]] DataBufferHandle createBuffer(const DataBufferDesc& d, const void* initialData = nullptr);

        /// \copydoc IRenderDevice::destroyDataBuffer
        void destroyBuffer(DataBufferHandle h);

        // For persistent buffer frames:
        // void allocRing(DataBufferHandle h, size_t size, size_t align, uint64_t* outOffset);
        // void advanceBufferFrames();

        /// \copydoc IRenderDevice::updateDataBufferSubData
         void updateSubData(DataBufferHandle h, uint64_t offset, uint64_t size, const void* data);

        // void updateWholeData(DataBufferHandle h, uint64_t size, const void* data);

        /// \copydoc IRenderDevice::bindDataBufferBase
        void bindBase(DataBufferHandle h);

        // For ring:
        // void bindRange(DataBufferHandle h, uint64_t offset, uint64_t size);

    private:
        std::unordered_map<DataBufferHandle, LGLDataBufferRecord> createdBuffers;
    };

} // namespace LaurelEye::Graphics
