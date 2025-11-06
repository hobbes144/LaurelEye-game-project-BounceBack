/// @file   ShadowPass.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-31-2025
/// @brief This pass is used to render all the shadows.

#pragma once

#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"
#include "LaurelEyeEngine/math/Matrix4.h"

namespace LaurelEye::Graphics {

    class FrameContext;
    class RenderResources;

    /// @class ShadowPass
    /// @brief Concrete implementation of a shadow render pass.
    ///
    /// The `ShadowPass` is used to write the depth to a framebuffer.
    /// Note that it does not handle transparent objects. They are treated as
    /// opaque.
    class ShadowPass : public IRenderPass {
    public:
        struct Properties {
            Matrix4 ProjViewMatrix;
        };

        /// @brief Default constructor.
        ShadowPass() : IRenderPass() {}

        /// \copydoc IRenderPass::setup
        void setup(RenderResources& rs) override;
        /// \copydoc IRenderPass::execute
        void execute(const FrameContext& ctx) override;

        void setProperties(Properties _properties);
    protected:
        Properties properties;
    };

} // namespace LaurelEye::Graphics
