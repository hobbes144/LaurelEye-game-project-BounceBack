/// @file   DeferredRenderPass.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    11-06-2025
/// @brief  Render pass to perform deferred rendering

#pragma once

#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

namespace LaurelEye::Graphics {
    class Mesh;

    class DeferredRenderPass : public IRenderPass {
    public:
        DeferredRenderPass()
            : IRenderPass() {}

        void setup(RenderResources& rs) override;

        void execute(const FrameContext& ctx) override;

    private:
        TextureHandle sunShadowTex = InvalidTexture;
        TextureHandle gbufferPosition = InvalidTexture;
        TextureHandle gbufferNormal = InvalidTexture;
        TextureHandle gbufferDiffuse = InvalidTexture;
        TextureHandle gbufferSpecular = InvalidTexture;
        std::shared_ptr<Mesh> screenQuad;
    };
} // namespace LaurelEye::Graphics
