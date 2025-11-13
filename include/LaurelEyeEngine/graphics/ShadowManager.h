/// @file   ShadowManager.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    04-21-2025
/// @brief Shadow Manager class to handle creation and updation of shadow info

#pragma once

#include "LaurelEyeEngine/graphics/resources/Shadow.h"

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Vector3.h"

#include "LaurelEyeEngine/graphics/renderpass/ShadowPass.h"
#include <cstdint>
// #include "MomentShadowMapPass.h"

namespace LaurelEye::Graphics {

    class RenderResources;
    class PointLight;
    class DirectionalLight;
    class GlobalLights;
    class LocalLights;

    class ShadowManager {
    public:
        /// @struct ShadowResource
        /// @brief Structure containing information about a Shadow's resources.
        ///
        /// This structure holds a handle to a shadow, its descriptor, and
        /// an optional tag for categorization.
        ///
        /// Handle 0 is reserved for the sun shadow.
        ///
        struct ShadowInfo {
            ShadowDesc desc{};
            Vector3 lastLightPosDir;
            ShadowHandle handle;
            ShadowSourceType source;
            std::string tag;
            // Lifetime life{Lifetime::Persistent};
        };

        struct ShadowResource {
            Matrix4 shadowMatrix;
            FramebufferHandle framebuffer = Framebuffer::InvalidFramebuffer;
            TextureHandle texture = InvalidTexture;
            uint32_t lightIndex;
            uint32_t type = (uint32_t)ShadowType::Simple;
        };

        struct ShadowManagerParams {
            float near = 1.0f;
            float far = 500.0f;
            ShadowType defaultType = ShadowType::Simple;
        };

        ShadowManager() = delete;

        ShadowManager(RenderResources* _rs);

        void update(const FrameContext& ctx, GlobalLights* globalLights, LocalLights* lights);

        ShadowHandle addShadow(
            uint32_t lightIndex, PointLight* light, const ShadowDesc& d);
        ShadowHandle addShadow(
            uint32_t lightIndex, DirectionalLight* light, const ShadowDesc& d);

        void updateShadow(const FrameContext& ctx, ShadowHandle h, const Vector3& posDir);

        void bindShadowBuffer() const;

        const std::vector<ShadowInfo>& getShadows() const { return shadows; }
        const ShadowInfo& getShadow(ShadowHandle h) const { return shadows[h]; }
        const std::vector<ShadowResource>& getShadowResources() const { return shadowResources; }
        const ShadowResource& getShadowResource(ShadowHandle h) const { return shadowResources[h]; }

        // void setMomentWidth(int width);
        // void updateMomentShadowMap(ShadowInfo& shadow);

    private:
        ShadowManagerParams params{};

        int momentWidth = -1;
        DataBufferHandle momentUBO;

        const float alpha = 0.001f;

        // maps to LocalLights->pointLights[i].shadowIndex
        std::vector<ShadowInfo> shadows;
        std::vector<ShadowResource> shadowResources;
        ShadowInfo sunShadow;
        RenderResources* rs;
        DataBufferHandle shadowResourcesBuffer = DataBuffer::InvalidDataBuffer;

        ShadowPass shadowPass;
        // MomentShadowMapPass momentShadowMapPass;

        // void calculateMomentWeights(int width);
    };

} // namespace LaurelEye::Graphics
