/*!****************************************************************************
 * \file   SkydomePass.cpp
 * \author Anish Murthy (anish.murthy.dev@gmail.com)
 * \par    **DigiPen Email**
 *    anish.murthy@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   04-18-2025
 *
 * Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
 *
 *****************************************************************************/
#include "LaurelEyeEngine/graphics/renderpass/SkydomePass.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"


namespace LaurelEye::Graphics {
    void SkydomePass::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("../../../assets/shaders/Skydome.frag\n../../../assets/shaders/Skydome.vert");

        sphereMesh = (Mesh::createSphereMesh("sphere", 32));
        scale = (Matrix4::scale(Vector3(2000.0f)));
    }

    void SkydomePass::execute(const FrameContext& ctx) {
        glDepthMask(GL_FALSE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader->use();

        shader->bindTexture(10, "skydomeTexture", skydomeTexture);

        sphereMesh->draw(GL_TRIANGLES);

        shader->unuse();


        glDepthMask(GL_TRUE);
    }

    void SkydomePass::addTexture(TextureHandle tex) {
        skydomeTexture = tex;
    }
}
