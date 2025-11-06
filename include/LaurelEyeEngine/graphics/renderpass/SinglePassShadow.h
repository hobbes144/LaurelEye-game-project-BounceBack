/// @file   SinglePassShadow.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    11-04-2025
/// @brief Shadowed Single Pass

#pragma once

#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

namespace LaurelEye::Graphics {

    class SinglePassShadow : public IRenderPass {
    public:
        /// @brief Default constructor.
        SinglePassShadow() : IRenderPass() {}

        /// \copydoc IRenderPass::setup
        void setup(RenderResources& rs) override;
        /// \copydoc IRenderPass::execute
        void execute(const FrameContext& ctx) override;

    private:
        TextureHandle sunShadowTex = InvalidTexture;
    };

} // namespace LaurelEye::Graphics
