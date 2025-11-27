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
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/graphics/resources/VertexArray.h"
#include "LaurelEyeEngine/graphics/surface/IWindowSurfaceProvider.h"

#include <string>
#include <unordered_map>

namespace LaurelEye::Graphics {

    class IRenderDevice;
    class RenderSystem;

    /// @struct DataBufferResource
    /// @brief Structure containing information about a data buffer resource.
    ///
    /// This structure holds a handle to a GPU data buffer, its descriptor, and
    /// optional tags used to organize or group related resources.
    /// Tags follow snake_case.
    struct DataBufferResource {
        DataBufferHandle handle{};
        DataBufferDesc desc{};
        std::vector<std::string> tags;

        // Lifetime life{Lifetime::Persistent};
    };

    struct VertexArrayResource {
        VertexArrayHandle handle{};
        VertexArrayDesc desc{};
        std::vector<std::string> tags;
    };

    /// @struct TextureResource
    /// @brief Structure containing information about a texture resource.
    ///
    /// This structure holds a handle to a GPU texture, its descriptor, and
    /// optional tags for categorization.
    /// Tags follow snake_case.
    struct TextureResource {
        TextureHandle handle{};
        TextureDesc desc{};
        std::vector<std::string> tags;

        // Lifetime life{Lifetime::Persistent};
    };

    /// @struct FramebufferResource
    /// @brief Structure containing information about a frame buffer resource.
    ///
    /// This structure holds a handle to a GPU texture, its descriptor, and
    /// optional tags for categorization.
    /// Tags follow snake_case.
    struct FramebufferResource {
        FramebufferHandle handle{};
        FramebufferDesc desc{};
        std::vector<std::string> tags;
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
        explicit RenderResources(IRenderDevice& dev, RenderSystem* rs);
        ~RenderResources();

        /// @brief Creates a GPU data buffer resource.
        ///
        /// @param name Unique name identifying the data buffer.
        /// @param d Descriptor containing size, usage, and format information.
        /// @param tag Optional tag used to group related resources.
        /// @return Handle to the created DataBuffer resource.
        [[nodiscard]] DataBufferHandle createDataBuffer(const std::string& name, const DataBufferDesc& d,
                                                        const std::vector<std::string>& tags = {}, const void* init = nullptr);
        [[nodiscard]] DataBufferHandle createDataBuffer(const std::string& name, const DataBufferDesc& d,
                                                        const std::string& tag, const void* init = nullptr);
        /// @brief Retrieves a handle to an existing data buffer.
        ///
        /// @param name Name of the buffer to retrieve.
        /// @return Handle to the DataBuffer resource, or an invalid handle if not found.
        DataBufferHandle dataBuffer(const std::string& name);
        /// @brief Destroys a data buffer resource by name.
        ///
        /// @param name Name of the buffer to destroy.
        void destroyDataBuffer(const std::string& name);
        void destroyDataBuffer(DataBufferHandle h);
        // void resizeDataBuffer(const std::string& name, uint32_t size);
        void updateDataBuffer(const std::string& name,
                              uint64_t offset,
                              uint64_t size,
                              const void* data);
        void updateDataBuffer(DataBufferHandle h,
                              uint64_t offset,
                              uint64_t size,
                              const void* data);

        [[nodiscard]] VertexArrayHandle createVertexArray(const std::string& name, const VertexArrayDesc& d,
                                                          const std::vector<std::string>& tags = {});
        VertexArrayHandle vertexArray(const std::string& name);
        void destroyVertexArray(const std::string& name);
        void destroyVertexArray(VertexArrayHandle h);

        [[nodiscard]] TextureHandle createTexture(const std::string& name, const TextureDesc& d,
                                                  const std::vector<std::string>& tags = {}, const void* init = nullptr);
        [[nodiscard]] TextureHandle createTexture(const std::string& name, const TextureDesc& d,
                                                  const std::string& tag, const void* init = nullptr);
        [[nodiscard]] TextureHandle createScreenSizeTexture(const std::string& name,
                                                            const std::vector<std::string>& tags = {},
                                                            const SampleCount& samples = SampleCount::X1);
        TextureHandle texture(const std::string& name);
        void destroyTexture(const std::string& name);
        void resizeTexture(const std::string& name, const SizeRegistry& size);

        [[nodiscard]] FramebufferHandle createFramebuffer(const std::string& name, const FramebufferDesc& d,
                                                          const std::vector<std::string>& tags = {});
        [[nodiscard]] FramebufferHandle createFramebuffer(const std::string& name, const FramebufferDesc& d,
                                                          const std::string& tag);
        [[nodiscard]] TextureHandle createScreenSizeFramebuffer(const std::string& name,
                                                                const std::vector<std::string>& textureNames,
                                                                const std::vector<std::string>& tags = {},
                                                                const SampleCount& samples = SampleCount::X1);
        FramebufferHandle framebuffer(const std::string& name);
        void destroyFramebuffer(const std::string& name);
        void resizeFramebuffer(const std::string& name, const SizeRegistry& size);
        // void rebuildFramebuffer(const std::string& name, const FramebufferDesc& d);

        // Add resizeTag somewhere that resizes all resources with a specific tag.
        void resizeTag(const std::string& tag, const SizeRegistry& size);
        void resizeScreenSize(SurfaceHandle h, const SizeRegistry& size);

        // void destroy(const std::string& name);
        // void destroyTag(const std::string& tag);

        // NOTE: I'm unsure if this should be allowed
        IRenderDevice& getRenderDevice();

        void setSurfaceSize(SurfaceHandle h, const SizeRegistry& size);
        const SizeRegistry& getSurfaceSize(SurfaceHandle h = 0) const;

    private:
        /// @brief Reference to the underlying render device responsible for actual GPU operations.
        IRenderDevice& device;

        // Window size, later make a better system to handle more windows.
        SizeRegistry size;

        /// @brief Map of data buffer names to their corresponding resource metadata.
        std::unordered_map<std::string, DataBufferResource> dataBuffers;
        std::unordered_map<std::string, VertexArrayResource> vertexArrays;
        /// @brief Map of texture names to their corresponding resource metadata.
        std::unordered_map<std::string, TextureResource> textures;
        /// @brief Map of framebuffer names to their corresponding handles.
        std::unordered_map<std::string, FramebufferResource> framebuffers;
    };
} // namespace LaurelEye::Graphics
