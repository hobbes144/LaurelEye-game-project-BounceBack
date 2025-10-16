/// @file   FrameBufferTypes.h
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

#include <cstdint>

namespace LaurelEye::Graphics {

    /// @typedef FrameBufferHandle
    /// @brief Unique identifier for a framebuffer resource managed by the renderer.
    ///
    /// A `FrameBufferHandle` acts as a lightweight reference to a GPU-side framebuffer.
    /// Invalid or uninitialized handles should be represented as zero or a sentinel value.
    using FrameBufferHandle = uint32_t;

    /// @enum FrameBufferAttachmentType
    /// @brief Describes the type of attachment bound to a framebuffer.
    ///
    /// Framebuffers may contain color, depth, stencil, or combined depth-stencil attachments.
    /// These determine what kind of data is written during rendering passes.
    enum class FrameBufferAttachmentType { Color,
                                           Depth,
                                           Stencil,
                                           DepthStencil };

    /// @struct AttachmentDesc
    /// @brief Describes a single framebuffer attachment and its configuration.
    ///
    /// This structure defines the attachment type and can optionally specify additional
    /// parameters such as texture format, mip level, and sample count for multisampling.
    struct AttachmentDesc {
        FrameBufferAttachmentType type = FrameBufferAttachmentType::Color;
        // TextureHandle texture = 0;                   // optional: if nonzero, use existing texture
        // TextureFormat format = TextureFormat::RGBA8; // used if texture==0
        // SampleCount samples = SampleCount::X1;
        // uint32_t mipLevel = 0;        // for texture arrays / mips
        // uint32_t layer = 0;           // for array/cubemap layers
        // bool useRenderbuffer = false; // depth-only RBs are often faster
    };

    /// @struct FrameBufferDesc
    /// @brief Defines the layout and configuration of a framebuffer.
    ///
    /// The framebuffer descriptor aggregates one or more attachment descriptions.
    /// It can later be used by the rendering device to create an actual framebuffer object.
    struct FrameBufferDesc {
    };
} // namespace LaurelEye::Graphics
