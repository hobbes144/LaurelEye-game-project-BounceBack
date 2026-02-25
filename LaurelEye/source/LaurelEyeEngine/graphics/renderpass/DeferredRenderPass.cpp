/// @file   DeferredRenderPass.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    11-06-2025
/// @brief  Deferred Render Pass

#include "LaurelEyeEngine/graphics/renderpass/DeferredRenderPass.h"
#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderDevice.h"
#include "LaurelEyeEngine/graphics/RenderStateSaver.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"
#include "LaurelEyeEngine/graphics/ShadowManager.h"

namespace LaurelEye::Graphics {
    void DeferredRenderPass::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("../../../assets/shaders/DeferredRender.frag\n../../../assets/shaders/DeferredRender.vert");

        screenQuad = (Mesh::createSquareMesh("screenQuad", 1.0f));
    }

    void DeferredRenderPass::execute(const FrameContext& ctx) {
        RenderStateSaver rs{&ctx.device};
        const SizeRegistry& size = ctx.resources.getSurfaceSize(0);
        glViewport(0, 0, size.width, size.height);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

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

        if ( !isValidTexture(sunShadowTex) ) {
            for ( const auto& shadow : ctx.shadowManager->getShadows() ) {
                if ( shadow.source == ShadowSourceType::Directional ) {
                    sunShadowTex = ctx.shadowManager->getShadowResource(shadow.handle).texture;
                    break;
                }
            }
        }

        shader->bindTexture(0, "gbuffer_position", gbufferPosition);
        shader->bindTexture(1, "gbuffer_normal", gbufferNormal);
        shader->bindTexture(2, "gbuffer_diffuse", gbufferDiffuse);
        shader->bindTexture(3, "gbuffer_specular", gbufferSpecular);
        if ( isValidTexture(sunShadowTex) ) shader->bindTexture(10, "ShadowMap", sunShadowTex);

        screenQuad->draw(GL_TRIANGLES);

        shader->unuse();

        if ( isValidTexture(sunShadowTex) ) shader->unbindTexture(10);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);

        // glBindFramebuffer(GL_READ_FRAMEBUFFER, ctx.resources.framebuffer("gbuffer"));
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // or your post-FBO if you have one
        //
        // // 2. Blit only depth
        // glBlitFramebuffer(
        //     0, 0, 1280, 720, // src rect
        //     0, 0, 1280, 720, // dst rect
        //     GL_DEPTH_BUFFER_BIT, // what to copy
        //     GL_NEAREST           // filter (must be NEAREST for depth)
        // );
        //
        // // 3. Go back to your main draw target if needed
        // glBindFramebuffer(GL_FRAMEBUFFER, 0); // or your forward FBO
    }
} // namespace LaurelEye::Graphics
