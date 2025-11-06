/// @file   IRenderPass.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///    anish.murthy@digipen.edu
/// @par    **Course**
///    GAM550
/// @date   10-04-2025
/// @brief  Render Pass class interface
#pragma once

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"

#include <memory>

namespace LaurelEye::Graphics {

    class FrameContext;
    class RenderResources;
    class Shader;

    /// \brief Render Pass class used to render using a shader
    ///
    /// ## Usage:
    ///
    /// This class is meant to be used to render using a shader. Each shading pass
    /// for rendering should be handled using a RenderPass.
    ///
    /// Typically after the shader and resources are set, this class should be
    /// registered to the RenderGraph, which will handle rendering.
    ///
    /// ## Pre-registration calls:
    ///
    /// - setResource(const std::string& name, void* value)
    ///
    /// If FBO functionality is needed,
    /// - setFBO(std::shared_ptr<FBO> FBO)
    ///
    /// ## Explanation:
    ///
    /// The class provides an interface for the Shader class. It stores the required
    /// resources to run the specific pass, and applies them when apply() is run.
    ///
    /// FBO functionality is supported if needed, and is diabled by default unless
    /// the setFBO() call is used.
    class IRenderPass : public std::enable_shared_from_this<IRenderPass> {
    public:
        /// @brief Virtual destructor for safe cleanup.
        virtual ~IRenderPass() = default;

        /// @brief Sets up the render pass with the necessary resources.
        ///
        /// Called once before execution (per frame or per scene, depending on context)
        /// to initialize GPU resources, bindings, or shader state needed by the pass.
        ///
        /// @param rs Reference to the `RenderResources` containing global GPU data
        ///        such as buffers, textures, and frame constants.
        virtual void setup(RenderResources& rs) = 0;

        /// @brief Executes the render pass.
        ///
        /// Called every frame or as part of a specific rendering stage.
        /// Responsible for binding the correct shader, updating data buffers,
        /// and drawing visible renderables.
        ///
        /// @param ctx The `FrameContext` containing per-frame data such as camera,
        ///        viewport, and scene information.
        virtual void execute(const FrameContext& ctx) = 0;

    protected:
        /// @brief Shader used by this render pass.
        std::shared_ptr<Shader> shader = nullptr;

        /// @brief Handle to the GPU data buffer containing pass-specific properties.
        DataBufferHandle propertiesHandle = DataBuffer::InvalidDataBuffer;
        bool propertiesDirty = false;

        /// @brief Draws all renderable entities using the given shader.
        ///
        /// This helper function can be called by derived classes to iterate
        /// through all active renderables in the frame and issue draw calls.
        ///
        /// @param ctx Frame context containing the active scene and render data.
        /// @param shader The shader program to use when drawing the renderables.
        virtual void drawRenderables(const FrameContext& ctx, std::shared_ptr<Shader> shader);
    };

} // namespace LaurelEye::Graphics
