/// @file   LGLRenderDevice.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-08-2025
/// @brief Render Device that handles all OpenGL interactions for the engine.

#pragma once

#include "LaurelEyeEngine/graphics/device/IRenderDevice.h"

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/VertexArray.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

#include <glad/glad.h>

#include <memory>

namespace LaurelEye::Graphics {

    class LGLDataBufferFactory;
    class LGLVertexArrayFactory;
    class LGLTextureFactory;
    class LGLFramebufferFactory;

    // This is a helper function to catch OpenGL errors.
#define GL_CHECKERROR(msg)                                                                      \
    {                                                                                           \
        GLenum err;                                                                             \
        while ( (err = glGetError()) != GL_NO_ERROR ) {                                         \
            fprintf(stderr, "OpenGL error at %s:%d - %s (%d)\n", __FILE__, __LINE__, msg, err); \
        }                                                                                       \
    }

    /// @brief OpenGL debug info callback function.
    ///
    /// @param source Notification source.
    /// @param type Notification type.
    /// @param id Notifcation ID.
    /// @param severity Notification severity.
    /// @param length ??.
    /// @param message Notification message.
    /// @param userParam ??.
    void APIENTRY openglCallbackFunction(
        GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam);

    /// @class LGLRenderDevice
    /// @brief OpenGL Render Device for the engine to interact with the GPU.
    ///
    /// Ideally all OpenGL calls occur only through this Device. We shouldn't
    /// see the glad or gl header includes anywhere else.
    ///
    /// To keep this class lean and stick to SRP, this is only an interface
    /// for the implementation classes, which are the Factories.
    class LGLRenderDevice final : public IRenderDevice {
    public:
        LGLRenderDevice();
        // Destructor defined in cpp to allow for forward declarations to work with unique_ptr.
        ~LGLRenderDevice();

        /// \copydoc IRenderDevice::initialize
        void initialize() override;
        /// \copydoc IRenderDevice::shutdown
        void shutdown() override;

        void clear() override;

        void setViewport(const Viewport& viewport, bool force) override;
        void setDepthState(const DepthState& depthState, bool force) override;
        void setBlendState(const BlendState& blendState, bool force) override;

        RenderState getCurrentState() override;
        void setState(const RenderState& rs, bool force = false) override;

        /* DataBuffer operations (UBOs/SSBOs) */

        /// \copydoc IRenderDevice::createDataBuffer
        [[nodiscard]] DataBufferHandle createDataBuffer(const DataBufferDesc& d, const void* initialData = nullptr) override;

        /// \copydoc IRenderDevice::destroyDataBuffer
        void destroyDataBuffer(DataBufferHandle h) override;

        /// \copydoc IRenderDevice::updateDataBufferSubData
        void updateDataBufferSubData(DataBufferHandle h, uint64_t offset, uint64_t size, const void* data) override;

        /// \copydoc IRenderDevice::bindDataBufferBase
        void bindDataBufferBase(DataBufferHandle h) override;

        // TODO: virtual void destroyAllDataBufffers() = 0;

        /* Vertex Array operations */
        [[nodiscard]] VertexArrayHandle createVertexArray(const VertexArrayDesc& d) override;
        void destroyVertexArray(VertexArrayHandle h) override;

        void bindVertexArray(VertexArrayHandle h) override;

        /* Texture operations */

        /// \copydoc IRenderDevice::createTexture
        [[nodiscard]] TextureHandle createTexture(const TextureDesc& d, const void* init = nullptr) override;

        /// \copydoc IRenderDevice::generateTextureMips
        void generateTextureMips(TextureHandle h) override;

        /// \copydoc IRenderDevice::resizeTexture
        void resizeTexture(TextureHandle h, uint32_t newW, uint32_t newH, uint32_t newD = 1) override;

        /// \copydoc IRenderDevice::setTextureSampler
        void setTextureSampler(TextureHandle h, const SamplerDesc& sDesc) override;

        /// \copydoc IRenderDevice::destroyTexture
        void destroyTexture(TextureHandle h) override;

        /// \copydoc IRenderDevice::destroyAllTextures
        void destroyAllTextures() override;

        void bindTexture(TextureHandle h, uint32_t textureUnit) override;

        /* Texture operations */

        /// \copydoc IRenderDevice::createFramebuffer
        [[nodiscard]] FramebufferHandle createFramebuffer(const FramebufferDesc& d) override;

        void finalizeFramebuffer(FramebufferHandle h) override;

        /// \copydoc IRenderDevice::destroyFramebuffer
        void destroyFramebuffer(FramebufferHandle h) override;

        /// \copydoc IRenderDevice::bindFramebufferBase
        void bindFramebufferBase(FramebufferHandle h) override;

        uint32_t attachTexturetoFramebuffer(FramebufferHandle h, const FramebufferAttachmentDesc& d) override;

        void resizeFramebuffer(FramebufferHandle h, uint32_t width, uint32_t height) override;

        void blitFramebuffers(FramebufferHandle source, FramebufferHandle dest, bool depthOnly = false) override;

    private:
        std::unique_ptr<LGLDataBufferFactory> dataBufferFactory;
        std::unique_ptr<LGLVertexArrayFactory> vertexArrayFactory;
        std::unique_ptr<LGLTextureFactory> textureFactory;
        std::unique_ptr<LGLFramebufferFactory> framebufferFactory;

        GLbitfield clearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    };

} // namespace LaurelEye::Graphics
