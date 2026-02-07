#pragma once
/// @file   ShadowSkinnedPass.h
/// @author Martin Chow
/// @par    **DigiPen Email**
///     martin.chow@digipen.edu
/// @date    2-2-2025
/// @brief This pass is used to render all the shadows with skeleton logic.

#pragma once

#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"
#include "LaurelEyeEngine/math/Matrix4.h"

namespace LaurelEye::Graphics {

    struct FrameContext;
    class RenderResources;

    /// @class ShadowPass
    /// @brief Concrete implementation of a shadow render pass.
    ///
    /// The `ShadowPass` is used to write the depth to a framebuffer.
    /// Note that it does not handle transparent objects. They are treated as
    /// opaque.
    class ShadowSkinnedPass : public IRenderPass {
    public:
        struct Properties {
            Matrix4 ProjViewMatrix;
        };

        /// @brief Default constructor.
        ShadowSkinnedPass() : IRenderPass() {}

        /// \copydoc IRenderPass::setup
        void setup(RenderResources& rs) override;
        /// \copydoc IRenderPass::execute
        void execute(const FrameContext& ctx) override;

        void setProperties(Properties _properties);

    private:
        void drawSkinnedRenderables(const FrameContext& ctx, std::shared_ptr<Shader> shader);

    protected:
        Properties properties;
    };

} // namespace LaurelEye::Graphics
