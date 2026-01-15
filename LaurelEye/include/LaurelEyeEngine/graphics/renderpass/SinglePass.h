/// @file   SinglePass.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date   10-08-2025
/// @brief  A simple render pass implementation that performs a single rendering stage.
///
/// This render pass represents a minimal implementation of the `IRenderPass` interface,
/// providing the standard setup and execute stages. It is typically used for simple
/// forward-rendering operations or passes that do not require multiple sub-passes or
/// complex GPU state management.

#pragma once

#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"

namespace LaurelEye::Graphics {

    struct FrameContext;
    class RenderResources;

    /// @class SinglePass
    /// @brief Concrete implementation of a single-stage render pass.
    ///
    /// The `SinglePass` class defines a basic rendering pass derived from `IRenderPass`.
    /// It encapsulates a single setup and execution cycle, making it suitable for
    /// forward-rendering pipelines, post-processing passes, or testing individual shaders.
    class SinglePass : public IRenderPass {
    public:
        /// @brief Default constructor.
        SinglePass() : IRenderPass() {}

        /// \copydoc IRenderPass::setup
        void setup(RenderResources& rs) override;
        /// \copydoc IRenderPass::execute
        void execute(const FrameContext& ctx) override;
    };

} // namespace LaurelEye::Graphics
