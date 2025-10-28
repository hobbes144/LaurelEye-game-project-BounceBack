/// @file   RenderResources.h
/// @author Anish Murthy
/// @par    **DigiPen Email**
///    anish.murthy@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   10-09-2025
/// @brief
///
///

#pragma once

#include "LaurelEyeEngine/graphics/device/IRenderDevice.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/FrameBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

#include <string>
#include <unordered_map>

namespace LaurelEye::Graphics {

    class IRenderDevice;

    /// @struct DataBufferResource
    /// @brief Structure containing information about a data buffer resource.
    ///
    /// This structure holds a handle to a GPU data buffer, its descriptor, and
    /// an optional tag used to organize or group related resources.
    struct DataBufferResource {
        DataBufferHandle handle{};
        DataBufferDesc desc{};
        std::string tag;
        // Lifetime life{Lifetime::Persistent};
    };

    /// @struct TextureResource
    /// @brief Structure containing information about a texture resource.
    ///
    /// This structure holds a handle to a GPU texture, its descriptor, and
    /// an optional tag for categorization.
    struct TextureResource {
        TextureHandle handle{};
        TextureDesc desc{};
        std::string tag;
        // Lifetime life{Lifetime::Persistent};
    };

    /// @class RenderResources
    /// @brief Creates and manages GPU resources for rendering.
    ///
    /// Makes calls to the IRenderDevice to create the resources. No creation
    /// logic is owned by this class.
    ///
    /// This is to organize the resources for the rendering, but the actual
    /// resources may have managed objects behind the scenes in the device.
    ///
    /// The render passes will call the device directly to interact with the
    /// resources, only creation, destruction and maintenance is here.
    ///
    /// TODO: Consider adding Lifecycle to each of the objects, so we can have
    /// FrameTransient objects which are destroyed after a single frame.
    /// Also add "clearTransients()" for the same.
    ///
    class RenderResources {
    public:
        /// @brief Constructs a RenderResources manager.
        /// @param dev Reference to the active render device.
        explicit RenderResources(IRenderDevice& dev) : device(dev) {}

        /// @brief Creates a GPU data buffer resource.
        ///
        /// @param name Unique name identifying the data buffer.
        /// @param d Descriptor containing size, usage, and format information.
        /// @param tag Optional tag used to group related resources.
        /// @return Handle to the created DataBuffer resource.
        DataBufferHandle createDataBuffer(const std::string& name, const DataBufferDesc& d,
                const std::string& tag);
        TextureHandle createTexture(const std::string& name, const TextureDesc& d,
                 const std::string& tag, const void* init = nullptr);
        // FrameBufferHandle createFrameBuffer(const std::string& name, const FrameBufferDesc& d,
        //         const std::string& tag);

        // Add Textures create, get, resize and destroy here. Also add resizeTag.

        /// @brief Retrieves a handle to an existing data buffer.
        ///
        /// @param name Name of the buffer to retrieve.
        /// @return Handle to the DataBuffer resource, or an invalid handle if not found.
        DataBufferHandle dataBuffer(const std::string& name);
        TextureHandle texture(const std::string& name);
        // FrameBufferHandle frameBuffer(const std::string& name);

        // void rebuildFrameBuffer(const std::string& name, const FrameBufferDesc& d);

        /// @brief Destroys a data buffer resource by name.
        ///
        /// @param name Name of the buffer to destroy.
        void destroyDataBuffer(const std::string& name);
        void destroyTexture(const std::string& name);

        // void destroy(const std::string& name);
        // void destroyTag(const std::string& tag);

    private:
        /// @brief Reference to the underlying render device responsible for actual GPU operations.
        IRenderDevice& device;

        /// @brief Map of data buffer names to their corresponding resource metadata.
        std::unordered_map<std::string, DataBufferResource> databuffers;
        /// @brief Map of framebuffer names to their corresponding handles.
        std::unordered_map<std::string, FrameBufferHandle> framebuffers;
        /// @brief Map of texture names to their corresponding resource metadata.
        std::unordered_map<std::string, TextureResource> textures;
    };
}
