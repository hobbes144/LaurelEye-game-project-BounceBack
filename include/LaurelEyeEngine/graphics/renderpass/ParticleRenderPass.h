#pragma once

#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/GeometryBuffer.h"

namespace LaurelEye::Graphics {

    class ParticleRenderPass : public IRenderPass {
    public:
        /// @brief Default constructor.
        ParticleRenderPass()
            : IRenderPass() {}

        /// \copydoc IRenderPass::setup
        void setup(RenderResources& rs) override;
        /// \copydoc IRenderPass::execute
        void execute(const FrameContext& ctx) override;

        void updateNumParticles(unsigned int np);
        void updateParticleBuffer(std::shared_ptr<Graphics::GeometryBuffer> pb);

    private:
        unsigned int numParticles;
        std::shared_ptr<Graphics::GeometryBuffer> particleBuffer;

    };

} // namespace LaurelEye::Graphics
