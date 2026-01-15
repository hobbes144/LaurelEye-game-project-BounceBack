///
/// @file   DebugDrawRenderPass.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   11-04-2025
/// @brief  @file   ParticleRenderPass.cpp
///

#pragma once

#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/GeometryBuffer.h"

namespace LaurelEye::Graphics {

    class DebugDrawRenderPass : public IRenderPass {

    public:
        /// @brief Default Constructor
        DebugDrawRenderPass()
            : IRenderPass() {}

        /// \copydoc IRenderPass::setup
        void setup(RenderResources& rs) override;
        /// \copydoc IRenderPass::execute
        void execute(const FrameContext& ctx) override;

        void updateNumLines(unsigned int nl);
        void updateLineBuffer(std::shared_ptr<Graphics::GeometryBuffer> lb);

    private:
        unsigned int numLines;
        std::shared_ptr<Graphics::GeometryBuffer> lineBuffer;

        float lineWidthRange[2];

    };

}

