/// @file   GBufferPass.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    11-06-2025
/// @brief  GBuffer Pass

#include "LaurelEyeEngine/graphics/renderpass/GBufferPass.h"

#include "LaurelEyeEngine/graphics/device/glfw/LGLTextureFactory.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"

#include <GL/gl.h>

namespace LaurelEye::Graphics {

    void GBufferPass::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("GBufferPass", "../../../assets/shaders/GBuffer.frag\n../../../assets/shaders/GBuffer.vert");
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

    void GBufferPass::execute(const FrameContext& ctx) {

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
            shader->setInt("useTexture", 1);
            shader->bindTexture(10, "mainTexture", skydomeTexture);

            sphereMesh->draw(GL_TRIANGLES);
            glDepthMask(GL_TRUE);
            shader->unbindTexture(10);
        }
        shader->setInt("isSkydome", 0);

        drawRenderables(ctx, shader);

        ctx.device.bindFramebufferBase(0);
        ctx.device.blitFramebuffers(gbufferMultisampled, gbuffer);
    }
} // namespace LaurelEye::Graphics
