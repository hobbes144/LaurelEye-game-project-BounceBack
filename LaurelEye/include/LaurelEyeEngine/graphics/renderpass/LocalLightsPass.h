#pragma once

#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include <cstdint>
#include <memory>

namespace LaurelEye::Graphics {

    struct FrameContext;
    class RenderResources;

    /// @class LocalLightPass
    /// @brief Local light rendering pass
    ///
    class LocalLightsPass : public IRenderPass {
    public:
        /// @brief Default constructor.
        LocalLightsPass() : IRenderPass() {}

        /// \copydoc IRenderPass::setup
        void setup(RenderResources& rs) override;
        /// \copydoc IRenderPass::execute
        void execute(const FrameContext& ctx) override;

        void setLightCount(uint32_t _lightCount) { lightCount = _lightCount; }

    private:
        std::shared_ptr<Mesh> lightSphere;
        uint32_t lightCount = 0;

        TextureHandle sunShadowTex = InvalidTexture;
        TextureHandle gbufferPosition = InvalidTexture;
        TextureHandle gbufferNormal = InvalidTexture;
        TextureHandle gbufferDiffuse = InvalidTexture;
        TextureHandle gbufferSpecular = InvalidTexture;
    };

} // namespace LaurelEye::Graphics
