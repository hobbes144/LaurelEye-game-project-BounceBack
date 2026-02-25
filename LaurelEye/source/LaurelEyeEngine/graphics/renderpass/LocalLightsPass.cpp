/// @file   GBufferPass.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    11-06-2025
/// @brief  GBuffer Pass

#include "LaurelEyeEngine/graphics/renderpass/LocalLightsPass.h"

#include "LaurelEyeEngine/graphics/RenderStateSaver.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/GeometryBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"
#include "LaurelEyeEngine/graphics/ShadowManager.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

#include <GL/gl.h>

namespace LaurelEye::Graphics {

    void LocalLightsPass::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("../../../assets/shaders/LocalLights.frag\n../../../assets/shaders/LocalLights.vert");
        lightSphere = (Mesh::createSphereMesh("sphere", 32));
    }

    void LocalLightsPass::execute(const FrameContext& ctx) {
        // RenderStateSaver rs{&ctx.device};
        const SizeRegistry& size = ctx.resources.getSurfaceSize(0);
        // glViewport(0, 0, size.width, size.height);

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        shader->use();
        shader->setUInt("width", size.width);
        shader->setUInt("height", size.height);

        if ( !isValidTexture(gbufferPosition) ) {
            gbufferPosition = ctx.resources.texture("gbuffer_position");
            assert(isValidTexture(gbufferPosition) &&
                   "LAURELEYE::RENDER_SYSTEM::DEFERRED_RENDER_PASS::GBUFFER_POSITION_MISSING");
        }
        if ( !isValidTexture(gbufferNormal) ) {
            gbufferNormal = ctx.resources.texture("gbuffer_normal");
            assert(isValidTexture(gbufferNormal) &&
                   "LAURELEYE::RENDER_SYSTEM::DEFERRED_RENDER_PASS::GBUFFER_NORMAL_MISSING");
        }
        if ( !isValidTexture(gbufferDiffuse) ) {
            gbufferDiffuse = ctx.resources.texture("gbuffer_diffuse");
            assert(isValidTexture(gbufferDiffuse) &&
                   "LAURELEYE::RENDER_SYSTEM::DEFERRED_RENDER_PASS::GBUFFER_DIFFUSE_MISSING");
        }
        if ( !isValidTexture(gbufferSpecular) ) {
            gbufferSpecular = ctx.resources.texture("gbuffer_specular");
            assert(isValidTexture(gbufferSpecular) &&
                   "LAURELEYE::RENDER_SYSTEM::DEFERRED_RENDER_PASS::GBUFFER_SPECULAR_MISSING");
        }

        shader->bindTexture(0, "gbuffer_position", gbufferPosition);
        shader->bindTexture(1, "gbuffer_normal", gbufferNormal);
        shader->bindTexture(2, "gbuffer_diffuse", gbufferDiffuse);
        shader->bindTexture(3, "gbuffer_specular", gbufferSpecular);

        // clean up shadow textures
        for ( uint32_t i = 0; i < MAX_SHADOWS; ++i )
            localShadowTex[i] = InvalidTexture;

        uint32_t shadowSlot = 0;
        for ( const auto& shadow : ctx.shadowManager->getShadows() ) {
            if ( shadow.source == ShadowSourceType::Point ) {
                if ( shadowSlot < MAX_SHADOWS ) {
                    localShadowTex[shadowSlot] = ctx.shadowManager->getShadowResource(shadow.handle).texture;
                    shadowSlot++;
                }
            }
            LE_ASSERT("Graphics", shadowSlot <= MAX_SHADOWS,
                      "error::Local Light Pass-> execute , point light size is > Max_Shadows");
        }

        for ( uint32_t i = 0; i < shadowSlot; ++i ) {
            if ( isValidTexture(localShadowTex[i]) ) {
                shader->bindTexture(
                    10 + i,
                    "shadowCubeMaps[" + std::to_string(i) + "]",
                    localShadowTex[i],
                    TextureType::TextureCube);
            }
        }


        for ( size_t lightIndex = 0; lightIndex < lightCount; ++lightIndex ) {
            uint32_t uiLightIndex = static_cast<uint32_t>(lightIndex);
            shader->setUInt("lightIndex", uiLightIndex);
            lightSphere->draw(GL_TRIANGLES);
        }

        for ( uint32_t i = 0; i < shadowSlot; ++i ) {
            if ( isValidTexture(localShadowTex[i]) ) {
                shader->unbindTexture(
                    11 + i, TextureType::TextureCube);
            }
        }

        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
    }
} // namespace LaurelEye::Graphics
