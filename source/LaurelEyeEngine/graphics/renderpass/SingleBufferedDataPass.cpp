/// @file   SinglePass.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of SinglePass render pass

#include "LaurelEyeEngine/graphics/renderpass/SingleBufferedDataPass.h"

#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/Matrix4.h"

namespace LaurelEye::Graphics {

    void SingleBufferedDataPass::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("../../../assets/shaders/SinglePassWithBuffers.frag\n../../../assets/shaders/SinglePassWithBuffers.vert");
    }

    void SingleBufferedDataPass::execute(const FrameContext& ctx) {

        shader->use();

        // Lights
        // shader->setInt("mode", 2);
        // shader->setVec3("Light", Vector3(4.0f));
        // shader->setVec3("Ambient", Vector3(0.2f));

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
