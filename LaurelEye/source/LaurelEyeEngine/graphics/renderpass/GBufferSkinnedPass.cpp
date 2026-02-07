/// @file   GBufferSkinnedPass.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    11-29-2025
/// @brief GBuffer Pass for skinned models

#include "LaurelEyeEngine/graphics/renderpass/GBufferSkinnedPass.h"

#include "LaurelEyeEngine/graphics/device/glfw/LGLTextureFactory.h"
#include "LaurelEyeEngine/graphics/graphics_components/IRenderableComponent.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"

#include <GL/gl.h>

namespace LaurelEye::Graphics {

    void GBufferSkinnedPass::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("../../../assets/shaders/GBuffer.frag\n../../../assets/shaders/GBufferSkinned.vert");
        sphereMesh = (Mesh::createSphereMesh("sphere", 32));

        gbufferMultisampled = rs.createScreenSizeFramebuffer(
            "gbuffer_ms",
            {"gbuffer_ms_position",
             "gbuffer_ms_normal",
             "gbuffer_ms_diffuse",
             "gbuffer_ms_specular"},
            {"gbuffer_ms"},
            SampleCount::X4);
        gbuffer = rs.createScreenSizeFramebuffer(
            "gbuffer",
            {"gbuffer_position",
             "gbuffer_normal",
             "gbuffer_diffuse",
             "gbuffer_specular"},
            {"gbuffer"},
            SampleCount::X1);
    }

    void GBufferSkinnedPass::execute(const FrameContext& ctx) {

        SizeRegistry size = ctx.resources.getSurfaceSize();
        ctx.device.bindFramebufferBase(gbufferMultisampled);
        glViewport(0, 0, size.width, size.height);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        // By default, this clears color and depth.
        ctx.device.clear();

        shader->use();

        if ( isValidTexture(skydomeTexture) ) {
            glDepthMask(GL_FALSE);
            shader->setInt("isSkydome", 1);
            shader->setInt("isSkinned", 0);
            shader->setInt("useTexture", 1);
            shader->bindTexture(10, "mainTexture", skydomeTexture);

            sphereMesh->draw(GL_TRIANGLES);
            glDepthMask(GL_TRUE);
        }
        shader->setInt("isSkydome", 0);

        drawSkinnedRenderables(ctx, shader);

        ctx.device.bindFramebufferBase(0);
        ctx.device.blitFramebuffers(gbufferMultisampled, gbuffer);
    }

    void GBufferSkinnedPass::drawSkinnedRenderables(const FrameContext& ctx, std::shared_ptr<Shader> shader) {
        if ( isValidDataBuffer(propertiesHandle) ) ctx.device.bindDataBufferBase(propertiesHandle);

        for ( const auto& icomponent : ctx.objects ) {
            if ( !icomponent->isActive() ) continue;
            // Remove this when we fix FrameContext to send only renderables:
            auto component = (IRenderableComponent*)icomponent;

            if ( isValidMesh(component->GetMesh()) ) {
                component->GetMaterial()->setProperty(
                    "ModelMatrix",
                    component->GetBoundTransform()->getWorldTransform().getLocalMatrix());
                component->GetMaterial()->setProperty(
                    "InvModelMatrix",
                    component->GetBoundTransform()->getWorldTransform().getInverseLocalMatrix());

                component->GetMaterial()->apply(shader);

                if ( !isValidDataBuffer(component->skinDataBuffer) || !isValidDataBuffer(component->animationDataBuffer) ) {
                    shader->setInt("isSkinned", 0);
                }
                else {
                    ctx.device.bindDataBufferBase(component->skinDataBuffer);
                    ctx.device.bindDataBufferBase(component->animationDataBuffer);
                    shader->setInt("isSkinned", 1);
                }

                ctx.device.bindVertexArray(component->vao);
                glDrawElements(GL_TRIANGLES, component->indexCount, GL_UNSIGNED_INT, 0);
            }
        }
    }
} // namespace LaurelEye::Graphics
