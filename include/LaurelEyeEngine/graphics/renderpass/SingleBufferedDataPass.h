#pragma once

#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"

namespace LaurelEye::Graphics {

    class SingleBufferedDataPass : public IRenderPass {
    public:
        /// @brief Default constructor.
        SingleBufferedDataPass() : IRenderPass() {}

        /// \copydoc IRenderPass::setup
        void setup(RenderResources& rs) override;
        /// \copydoc IRenderPass::execute
        void execute(const FrameContext& ctx) override;
    };

} // namespace LaurelEye::Graphics
