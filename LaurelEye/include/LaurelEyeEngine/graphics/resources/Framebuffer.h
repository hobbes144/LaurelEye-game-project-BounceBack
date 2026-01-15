/// @file   FramebufferTypes.h

/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date   10-08-2025
/// @brief  Defines framebuffer handle types and attachment descriptors for render targets.
///
/// This header introduces the structures and enumerations used to define framebuffers
/// and their attachments within the LaurelEye rendering system. Framebuffers are GPU
/// objects that represent one or more textures or renderbuffers which serve as render
/// targets during a render pass.

#pragma once

#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

#include <cstdint>
#include <vector>

namespace LaurelEye::Graphics {

    /// @typedef FramebufferHandle
    /// @brief Unique identifier for a framebuffer resource managed by the renderer.
    ///
    /// A `FramebufferHandle` acts as a lightweight reference to a GPU-side framebuffer.
    /// Invalid or uninitialized handles should be represented as InvalidFramebuffer or UINT32_MAX.
    using FramebufferHandle = uint32_t;
    using FramebufferAttachmentIndex = uint32_t;

    namespace Framebuffer {
        static constexpr FramebufferHandle InvalidFramebuffer = UINT32_MAX;
    }

    inline bool isValidFramebuffer(FramebufferHandle h) noexcept {
        return h != Framebuffer::InvalidFramebuffer;
    }

    /// @enum FramebufferAttachmentType
    /// @brief Describes the type of attachment bound to a framebuffer.
    ///
    /// Framebuffers may contain color, depth, stencil, or combined depth-stencil attachments.
    /// These determine what kind of data is written during rendering passes.
    enum class FramebufferAttachmentType { Color,
                                           Depth,
                                           Stencil,
                                           DepthStencil };

    /// @struct FramebufferAttachmentDesc
    /// @brief Describes a single framebuffer attachment and its configuration.
    ///
    /// This structure defines the attachment type and can optionally specify additional
    /// parameters such as texture format, mip level, and sample count for multisampling.
    struct FramebufferAttachmentDesc {
        SizeRegistry size = SizeRegistry();                  // used if texture==0
        FramebufferAttachmentIndex colorAttachmentIndex = 0; // Only relevant if Color attachment.
                                                             // Note that repeated numbers leads to undefined behaviour.
        FramebufferAttachmentType type = FramebufferAttachmentType::Color;
        TextureHandle texture = InvalidTexture;                // optional: if nonzero, use existing texture
        TextureFormat format = TextureFormat::RGBA8;           // used if texture==0
        TextureMipMode mipMode = TextureMipMode::AutoGenerate; // used if texture==0
        uint32_t mipLevels = 1;                                // used if texture==0
        SampleCount samples = SampleCount::X1;
        // uint32_t layer = 0;           // for array/cubemap layers
    };

    /// @struct FramebufferDesc
    /// @brief Defines the layout and configuration of a framebuffer.
    ///
    /// The framebuffer descriptor aggregates one or more attachment descriptions.
    /// It is used by the rendering device to create an actual framebuffer object.
    ///
    /// When the width is set to -1, the screen buffer is treated as a screen
    /// size framebuffer (by RenderResources, Device calls do not support this
    /// directly).
    ///
    /// NOTE: Setting x and y is currently not supported.
    ///
    /// WARNING: If Attachment descriptions are added to FramebufferDesc during
    /// creation, these attachment textures will be created and registered
    /// directly to TextureManager, and will not be available from the
    /// RenderResources. Use this only when you do not need to use these
    /// attachments later.
    ///
    /// WARNING: Creating multiple Depth buffers in a single Framebuffer is
    /// unhandled and may lead to errors.
    struct FramebufferDesc {
        std::vector<FramebufferAttachmentDesc> attachments;
        SizeRegistry size;
        uint32_t x = 0;
        uint32_t y = 0;
    };
} // namespace LaurelEye::Graphics
