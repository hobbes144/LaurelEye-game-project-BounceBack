/// @file   UIPass.cpp
/// @author Daoming Wang (KeitherJinn@Outlook.com)
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date    10-30-2025
/// @brief Ui render pass implementation.

#include "LaurelEyeEngine/graphics/renderpass/UIPass.h"
#include "LaurelEyeEngine/UI/UIComponents/UIRenderComponent.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/Transform.h"

namespace LaurelEye::Graphics {

    void UIPass::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("../../../assets/shaders/UIPass.frag\n../../../assets/shaders/UIPass.vert");
        //textShader = ShaderManager::getInstance().loadFile("../../../assets/shaders/UIPass.frag\n../../../assets/shaders/UIPass.vert");
        Square = Mesh::createSquareMesh("Square", 1.0f);
    }

    void UIPass::execute(const FrameContext& ctx) {
        const SizeRegistry& size = ctx.resources.getSurfaceSize(0);
        // glViewport(0, 0, size.width, size.height);
        // glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_SCISSOR_TEST);

        glEnable(GL_BLEND);

        glBlendEquation(blendFactor.equation);
        //glBlendFunc(blendFactor.srcFactor, blendFactor.destFactor);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //drawRenderables(ctx, shader);
        Transform trans = Transform();
        //Loop Over UI Elements Drawing them in the shader as minimally as possible
        for ( IRenderComponent* rc : ctx.objects ) {
            auto temp = rc->getOwner()->getName();

            auto* uiComponent = dynamic_cast<UI::UIRenderComponent*>(rc);
            assert(uiComponent && "[ERROR - UIPass] UIPass received non-UIRenderComponent!");

            const UI::UITransformComponent* t = uiComponent->GetBoundTransform();
            assert(t && "[ERROR - UIPass] UIRenderComponent has no bound UITransformComponent!");

            const Rect& rec = t->GetResolvedRect();

            auto* uiMat = uiComponent->GetMaterial().get();
            //Set Model Matrix
            Matrix4 m = Matrix4::translation(Vector3(rec.center[0], rec.center[1], 0.0f)) *
                Matrix4::scale(Vector3(rec.size[0], rec.size[1], 1.0f));
            uiMat->setProperty("m_Model", m);

            //Set Screen Size Property
            uiMat->setProperty("u_ScreenSize", Vector2(size.width, size.height));

            //Set Color
            uiMat->setProperty("u_Color", uiComponent->GetColor());

            //Set Transparency
            uiMat->setProperty("transparency", uiComponent->GetTransparency());

            //Apply Texture
            uiMat->apply(shader);

            Square->draw(GL_TRIANGLES);
        }

        glBlendFunc(GL_ONE, GL_ONE);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

} // namespace LaurelEye::Graphics
