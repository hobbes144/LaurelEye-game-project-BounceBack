/// @file   IRenderDevice.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-08-2025
/// @brief Render Device Interface to allow the engine to interact with a
/// Graphics API.

#pragma once

#include "LaurelEyeEngine/graphics/resources/RenderState.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include <cstdint>
#include <string>

namespace LaurelEye::Graphics {

    /// @class IRenderDevice
    /// @brief The render device handles all backend API calls and provides a
    /// single interface for all rendering ops.
    ///
    /// This ideally means we only every import OpenGL or Vulkan here and
    /// nowhere else.
    ///
    /// All factories for resources like UBOs, SSBOs, FBOs, GeometryBuffers
    /// are handled here under subsystems.
    class IRenderDevice {
    public:
        virtual ~IRenderDevice() = default;

        /// @brief Initialize the Render Device.
        virtual void initialize() = 0;
        /// @brief Shutdown the Render Device.
        virtual void shutdown() = 0;

        virtual void clear() = 0;

        virtual void setViewport(const Viewport& viewport, bool force) = 0;
        virtual void setDepthState(const DepthState& depthState, bool force) = 0;
        virtual void setBlendState(const BlendState& blendState, bool force) = 0;

        virtual RenderState getCurrentState() { return state; };
        virtual void setState(const RenderState& rs, bool force = false) = 0;

        // These are to be uncommented as we implement each of these.
        // Under the API specific RenderDevices, we want to have the factories
        // or managers create these.

        /* DataBuffer operations (UBOs/SSBOs) */

        /// @brief Create a Data Buffer.
        ///
        /// @param d Description of the buffer to be created.
        /// @param initialData Initial data for the buffer if any.
        ///
        /// Note that initialData must be at least DataBufferDesc::sizeBytes.
        /// Providing a pointer to less than this size leads to undefined
        /// behavior.
        ///
        /// @return Handle of the Data Buffer.
        [[nodiscard]] virtual DataBufferHandle createDataBuffer(const DataBufferDesc& d, const void* init = nullptr) = 0;

        /// @brief Destroy a Data Buffer.
        ///
        /// @param h Handle of the Data Buffer.
        virtual void destroyDataBuffer(DataBufferHandle h) = 0;

        /// @brief Update Data Buffer data without recreate or resize.
        ///
        /// Note: The new data must lie within the DataBufferDesc::sizeBytes.
        /// To store more data or shrink buffer, perform a delete and recreate.
        /// Resize functionality may be provided by `updateWholeData(...)`
        /// in the future.
        ///
        /// @param h Handle of the Data Buffer.
        /// @param offset Offset to begin writing to.
        /// @param size Size of data in bytes to write.
        /// @param data Pointer to data to be written.
        virtual void updateDataBufferSubData(DataBufferHandle h, uint64_t offset, uint64_t size, const void* data) = 0;

        /// @brief Bind a Data Buffer to use in a shader.
        ///
        /// The binding point it will bind to will be
        /// DataBufferDesc::bindingPoint.
        ///
        /// @param h Handle of the Data Buffer.
        virtual void bindDataBufferBase(DataBufferHandle h) = 0;

        // TODO: virtual void destroyAllDataBufffers() = 0;

        /* Texture operations */

        /// @brief Create a Texture.
        ///
        /// This function creates a GPU Texture. This may be an empty texture
        /// that can be filled by the GPU.
        ///
        /// If an image must be uploaded, ensure the TextureDesc is set
        /// correctly to recieve the data passed in initialData.
        ///
        /// @param d Description of the texture to be created.
        /// @param init Initial data of the texture.
        /// @return Handle of the Texture.
        [[nodiscard]] virtual TextureHandle createTexture(const TextureDesc& d, const void* init = nullptr) = 0;

        /// @brief Generate Texture Mipmaps.
        ///
        /// If TextureMipMode is set to TextureMipMode::AutoGenerate when
        /// creating the texture with init data, this is run automatically.
        ///
        /// @param h Handle of the Texture.
        virtual void generateTextureMips(TextureHandle h) = 0;

        /// @brief Resize a Texture.
        ///
        /// Note: Texture data integrity is not guaranteed after resize.
        ///
        /// @param h Handle of the Texture.
        /// @param newW New width of the texture.
        /// @param newH New height of the texture.
        /// @param newD New depth of the texture (unimplemented).
        virtual void resizeTexture(TextureHandle h, uint32_t newW, uint32_t newH, uint32_t newD = 1) = 0;

        /// @brief Set a Texture specific Sampler.
        ///
        /// This hard codes sampler info to a texture.
        /// Note: Modern OpenGL discourages doing this, it reduces flexibility.
        /// Instead use a created sampler and specify the sampler when binding
        /// a texture for use. (Functionality not currently implemented).
        ///
        /// @param h Handle of the Texture.
        /// @param sDesc Description of the Sampler.
        virtual void setTextureSampler(TextureHandle h, const SamplerDesc& sDesc) = 0;

        /// @brief Destroy a Texture.
        ///
        /// @param h Handle of the Texture.
        virtual void destroyTexture(TextureHandle h) = 0;

        /// @brief Destroy all created Textures.
        ///
        /// This is called by LGLRenderDevice::shutdown.
        virtual void destroyAllTextures() = 0;

        virtual void bindTexture(TextureHandle h, uint32_t textureUnit) = 0;

        /* Frame buffer operations */

        /// @brief Create a Data Buffer.
        ///
        /// @param d Description of the buffer to be created.
        /// @param initialData Initial data for the buffer if any.
        ///
        /// Note that initialData must be at least DataBufferDesc::sizeBytes.
        /// Providing a pointer to less than this size leads to undefined
        /// behavior.
        ///
        /// @return Handle of the Data Buffer.
        [[nodiscard]] virtual FramebufferHandle createFramebuffer(const FramebufferDesc& d) = 0;

        virtual void finalizeFramebuffer(FramebufferHandle h) = 0;

        /// @brief Destroy a Data Buffer.
        ///
        /// @param h Handle of the Data Buffer.
        virtual void destroyFramebuffer(FramebufferHandle h) = 0;

        /// @brief Bind a Data Buffer to use in a shader.
        ///
        /// The binding point it will bind to will be
        /// DataBufferDesc::bindingPoint.
        ///
        /// @param h Handle of the Data Buffer.
        virtual void bindFramebufferBase(FramebufferHandle h) = 0;

        virtual uint32_t attachTexturetoFramebuffer(FramebufferHandle h, const FramebufferAttachmentDesc& d) = 0;

        virtual void resizeFramebuffer(FramebufferHandle h, uint32_t width, uint32_t height) = 0;

        virtual void blitFramebuffers(FramebufferHandle source, FramebufferHandle dest) = 0;

        // TODO: virtual void destroyAllFrameBufffers() = 0;

        // virtual SamplerHandle createSampler(const SamplerDesc&) = 0;
        // virtual void destroy(BufferHandle) = 0; /* … */

        // This is final step if or when we separate compile plan and execute plan.
        // void execute(RenderPlan plan);

    protected:
        RenderState state{};
    };
} // namespace LaurelEye::Graphics
