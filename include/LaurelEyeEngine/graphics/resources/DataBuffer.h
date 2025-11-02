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
    /// @brief Sentinel value representing an invalid buffer handle.
    static constexpr DataBufferHandle InvalidDataBuffer = UINT32_MAX;
    /// @brief Checks whether a given buffer handle is valid.
    /// @param h The handle to validate.
    /// @return `true` if the handle refers to a valid buffer, otherwise `false`.
    inline bool isValidDataBuffer(DataBufferHandle h) noexcept {
        return h != InvalidDataBuffer;
    }

    /// @enum DataBufferType
    /// @brief Specifies the type of GPU data buffer.
    ///
    /// Determines how the buffer will be bound and accessed in the rendering pipeline.
    enum class DataBufferType { UBO,
                                SSBO };

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

} // namespace LaurelEye::Graphics
