/// @file   UIPass.cpp
/// @author Daoming Wang (KeitherJinn@Outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-30-2025
/// @brief Ui render pass implementation.

#include "LaurelEyeEngine/graphics/renderpass/UIPass.h"

#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Vector3.h"

namespace LaurelEye::Graphics {

    void UIPass::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("../../../assets/shaders/UIPass.frag\n../../../assets/shaders/UIPass.vert");
    }

    void UIPass::execute(const FrameContext& ctx) {
        // glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);

        glBlendEquation(blendFactor.equation);
        glBlendFunc(blendFactor.srcFactor, blendFactor.destFactor);

        drawRenderables(ctx, shader);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

} // namespace LaurelEye::Graphics
