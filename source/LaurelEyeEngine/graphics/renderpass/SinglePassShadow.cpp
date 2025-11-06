/// @file   SinglePassShadow.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    11-04-2025
/// @brief Single Pass with Shadows

#include "LaurelEyeEngine/graphics/renderpass/SinglePassShadow.h"

#include "LaurelEyeEngine/graphics/ShadowManager.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"
#include "LaurelEyeEngine/graphics/resources/Shadow.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

namespace LaurelEye::Graphics {

    void SinglePassShadow::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("../../../assets/shaders/SinglePassShadow.frag\n../../../assets/shaders/SinglePassShadow.vert");
    }

    void SinglePassShadow::execute(const FrameContext& ctx) {

        shader->use();

        if (!isValidTexture(sunShadowTex)) {
            for (const auto& shadow : ctx.shadowManager->getShadows()) {
                if (shadow.source == ShadowSourceType::Directional) {
                    sunShadowTex = ctx.shadowManager->getShadowResource(shadow.handle).texture;
                    break;
                }
            }
        }

        shader->bindTexture(10, "ShadowMap", sunShadowTex);

        // Cameras
        // shader->setMat4("WorldView", Matrix4::lookAt(Vector3(-50.f, 15.f, 0.f), Vector3(0.f)));
        // shader->setMat4("WorldInverse", Matrix4::inverse(Matrix4::lookAt(Vector3(-50.f, 15.f, 0.f), Vector3(0.f))));
        // float worldRy = 0.4f;
        // float worldRx = worldRy * 1280.f / 720.0f;
        // shader->setMat4("WorldProj", Matrix4::perspective(45.0f * 3.14159f / 180.0f, 1280.f / 720.f, 0.1f, 1000.0f));

        // Action
        drawRenderables(ctx, shader);
    }

} // namespace LaurelEye::Graphics
