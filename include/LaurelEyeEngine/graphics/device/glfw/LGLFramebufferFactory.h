/// @file   LGLDataBufferFactory.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-08-2025
/// @brief Framebuffer Factory for OpenGL.
///
#pragma once

#include "LaurelEyeEngine/graphics/device/glfw/LGLTextureFactory.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

#include <glad/glad.h>

#include <unordered_map>

namespace LaurelEye::Graphics {

    // TODO: Consider moving this to a separate Renderbuffer resource file.
    using RenderbufferHandle = uint32_t;
    static constexpr RenderbufferHandle InvalidRenderbuffer = UINT32_MAX;
    inline bool isValidRenderbuffer(RenderbufferHandle h) noexcept {
        return h != InvalidRenderbuffer;
    }

    struct FramebufferAttachment {
        FramebufferAttachmentDesc d{};
        TextureHandle texture = InvalidTexture;
        RenderbufferHandle rbo = InvalidRenderbuffer;
        GLenum glAttachment = 0;
    };

    /// @class LGLFramebufferRecord
    /// @brief Record of Frame Buffer's GPU and operational info.
    struct LGLFramebufferRecord {
        FramebufferDesc desc{};

        GLuint id = 0;

        std::vector<FramebufferAttachment> attachments;
    };

    /// @class LGLDataBufferFactory
    /// @brief Data Buffer Factory responsible for all UBO and SSBO GPU
    /// operations and lifecycle.
    class LGLFramebufferFactory {
    public:
        LGLFramebufferFactory(LGLTextureFactory* textureFactory);

        /// \copydoc IRenderDevice::createFramebuffer
        [[nodiscard]] FramebufferHandle create(const FramebufferDesc& d);

        /// \copydoc IRenderDevice::destroyFramebuffer
        void destroy(FramebufferHandle h);

        /// \copydoc IRenderDevice::bindFramebufferBase
        void bindBase(FramebufferHandle h);

        uint32_t attachTexture(FramebufferHandle h, const FramebufferAttachmentDesc& d);

    private:
        FramebufferAttachment createDepthAttachment(
            GLuint framebufferID, const SizeRegistry& size, const FramebufferAttachmentDesc& d);

        FramebufferAttachment createColorAttachment(
            GLuint framebufferID, const SizeRegistry& size, const FramebufferAttachmentDesc& d);

        std::unordered_map<FramebufferHandle, LGLFramebufferRecord> createdBuffers;
        LGLTextureFactory* textureFactory;

        int colorAttachmentIndex = 0;
    };

} // namespace LaurelEye::Graphics
