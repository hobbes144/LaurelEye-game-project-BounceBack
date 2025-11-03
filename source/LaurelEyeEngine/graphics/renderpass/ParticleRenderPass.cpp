///
/// @file   ParticleRenderPass.cpp
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   10-30-2025
/// @brief  @file   SinglePass.cpp
///

#include "LaurelEyeEngine/graphics/renderpass/ParticleRenderPass.h"

#include "LaurelEyeEngine/graphics/resources/GeometryBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"

namespace LaurelEye::Graphics {

    void ParticleRenderPass::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("../../../assets/shaders/ParticleRenderPass.frag\n../../../assets/shaders/ParticleRenderPass.vert");
    }

    void ParticleRenderPass::execute(const FrameContext& ctx) {

        if ( numParticles == 0 ) return;

        shader->use();
        // Action

        particleBuffer->bind();

        glClear(GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_PROGRAM_POINT_SIZE);

        glDrawArrays(GL_POINTS, 0, numParticles);

        glDisable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_DEPTH_TEST);
    }

    void ParticleRenderPass::updateNumParticles(unsigned int np) {
        numParticles = np;
    }

    void ParticleRenderPass::updateParticleBuffer(std::shared_ptr<Graphics::GeometryBuffer> pb) {
        particleBuffer = pb;
    }

} // namespace LaurelEye::Graphics
