/// @file   GBufferPass.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    11-06-2025
/// @brief  GBuffer Pass

#pragma once

#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"

namespace LaurelEye::Graphics {
    class Mesh;

    class GBufferPass : public IRenderPass {
    public:
        /// @brief Default constructor.
        GBufferPass() : IRenderPass() {}

        /// \copydoc IRenderPass::setup
        void setup(RenderResources& rs) override;
        /// \copydoc IRenderPass::execute
        void execute(const FrameContext& ctx) override;

        void addSkydome(TextureHandle tex) {
            skydomeTexture = tex;
        }

    private:
        FramebufferHandle gbuffer;
        FramebufferHandle gbufferMultisampled;
        TextureHandle skydomeTexture;
        std::shared_ptr<Mesh> sphereMesh;
    };
} // namespace LaurelEye::Graphics
