/// @file   DataBufferTypes.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date   10-08-2025
/// @brief  Defines data buffer types, handles, and descriptors for GPU-side storage.
///
/// This header provides type-safe definitions for GPU data buffers used within
/// the rendering system. It defines buffer handle types, buffer usage modes,
/// and a descriptor struct that can be used to create and configure GPU-side
/// uniform or shader storage buffers.

#pragma once

#include <cstdint>

namespace LaurelEye::Graphics {

    /// @typedef DataBufferHandle
    /// @brief Strongly typed identifier for GPU data buffers.
    ///
    /// Each `DataBufferHandle` uniquely represents a buffer allocated
    /// by the rendering backend. The value `InvalidDataBuffer` can be
    /// used to represent an uninitialized or invalid handle.
    using DataBufferHandle = uint32_t;

    namespace DataBuffer {
        /// @brief Sentinel value representing an invalid buffer handle.
        static constexpr DataBufferHandle InvalidDataBuffer = UINT32_MAX;

        /// @brief common binding points for uniform/storage buffers
        ///
        /// NOTE: These are only meaningful for UBO/SSBO `DataBufferType`s
        static constexpr uint32_t CameraDataBinding = 0;
        static constexpr uint32_t GlobalLightDataBinding = 1;
        static constexpr uint32_t LocalLightDataBinding = 2;
        static constexpr uint32_t ShaderDataBinding = 10;

    } // namespace DataBuffer

    /// @brief Checks whether a given buffer handle is valid.
    /// @param h The handle to validate.
    /// @return `true` if the handle refers to a valid buffer, otherwise `false`.
    inline bool isValidDataBuffer(DataBufferHandle h) noexcept {
        return h != DataBuffer::InvalidDataBuffer;
    }

    /// @enum DataBufferType
    /// @brief Specifies the type of GPU data buffer.
    ///
    /// Determines how the buffer will be bound and accessed in the rendering pipeline.
    enum class DataBufferType { UBO,
                                SSBO,
                                Vertex,
                                Index };

    /// @enum DataBufferUpdateMode
    /// @brief Describes how the data buffer will be updated during runtime.
    ///
    /// Controls whether the buffer uses persistent mapping or dynamic uploads.
    /// Persistent mapping allows CPU-GPU shared memory for faster access, while
    /// dynamic mode uses transient updates per frame.
    enum class DataBufferUpdateMode { PersistentMapped,
                                      Dynamic };

    /// @struct DataBufferDesc
    /// @brief Describes the properties and creation parameters for a GPU data buffer.
    ///
    /// Provides metadata required for initializing GPU-side data buffers, including
    /// type, update mode, and binding configuration.
    struct DataBufferDesc {
        DataBufferType type;
        DataBufferUpdateMode mode{DataBufferUpdateMode::Dynamic};
        uint64_t sizeBytes{0};
        uint32_t bindingPoint{0};
    };

    // Optional convenience helpers (nice for call-sites, but not required)
    inline DataBufferDesc makeUBODesc(uint64_t sizeBytes,
                                      uint32_t bindingPoint,
                                      DataBufferUpdateMode mode = DataBufferUpdateMode::Dynamic) {
        return DataBufferDesc{
            .type = DataBufferType::UBO,
            .mode = mode,
            .sizeBytes = sizeBytes,
            .bindingPoint = bindingPoint};
    }

    inline DataBufferDesc makeSSBODesc(uint64_t sizeBytes,
                                       uint32_t bindingPoint,
                                       DataBufferUpdateMode mode = DataBufferUpdateMode::Dynamic) {
        return DataBufferDesc{
            .type = DataBufferType::SSBO,
            .mode = mode,
            .sizeBytes = sizeBytes,
            .bindingPoint = bindingPoint};
    }

    inline DataBufferDesc makeVertexBufferDesc(uint64_t sizeBytes,
                                               DataBufferUpdateMode mode = DataBufferUpdateMode::Dynamic) {
        return DataBufferDesc{
            .type = DataBufferType::Vertex,
            .mode = mode,
            .sizeBytes = sizeBytes,
            .bindingPoint = 0 // ignored
        };
    }

    inline DataBufferDesc makeIndexBufferDesc(uint64_t sizeBytes,
                                              DataBufferUpdateMode mode = DataBufferUpdateMode::Dynamic) {
        return DataBufferDesc{
            .type = DataBufferType::Index,
            .mode = mode,
            .sizeBytes = sizeBytes,
            .bindingPoint = 0 // ignored
        };
    }

} // namespace LaurelEye::Graphics
