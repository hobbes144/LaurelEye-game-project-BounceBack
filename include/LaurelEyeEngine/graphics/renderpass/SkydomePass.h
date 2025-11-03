#pragma once

#include "LaurelEyeEngine/graphics/renderpass/IRenderPass.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/GeometryBuffer.h"

namespace LaurelEye::Graphics {
    class SkydomePass : public IRenderPass {
    public:
        SkydomePass()
            : IRenderPass() {}

        void addTexture(TextureHandle tex);
        void addTexture(const std::string& textureName);

        void setup(RenderResources& rs) override;

        void execute(const FrameContext& ctx) override;

        TextureHandle getTexture() const { return skydomeTexture; }
        void setTexture(TextureHandle tex) { skydomeTexture = tex; }
    private:
        TextureHandle skydomeTexture;
        std::shared_ptr<Mesh> sphereMesh;
        Matrix4 scale;

    };
} // namespace LaurelEye
