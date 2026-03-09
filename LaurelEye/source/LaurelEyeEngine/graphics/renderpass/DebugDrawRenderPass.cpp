///
/// @file   DebugDrawRenderPass.cpp
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   11-04-2025
/// @brief
///

#include "LaurelEyeEngine/graphics/renderpass/DebugDrawRenderPass.h"

#include "LaurelEyeEngine/graphics/resources/GeometryBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Shader.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"
#include <stdexcept>

namespace LaurelEye::Graphics {

    void DebugDrawRenderPass::setup(RenderResources& rs) {
        shader = ShaderManager::getInstance().loadFile("DebugDrawRenderPass", "../../../assets/shaders/DebugDraw.frag\n../../../assets/shaders/DebugDraw.vert");

        // Query once at init
        glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange); // e.g., returns {1.0f, 1.0f}
        std::cout << "lineWidthRange: " << lineWidthRange[0] << ", " << lineWidthRange[1] << std::endl;
        // GLfloat smoothRange[2];
        // glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, smoothRange);
    }

    void DebugDrawRenderPass::execute(const FrameContext& ctx) {

        if ( !lineBuffer || numLines == 0 )
            return;

        shader->use();

        if ( !lineBuffer ) {
            throw std::runtime_error("ERROR::DEBUG_DRAW_RENDER_PASS::lineBuffer not initialized");
        }

        lineBuffer->bind();

        // GL Commands
        // glDisable(GL_DEPTH_TEST);
        // glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f);
        glEnable(GL_LINE_SMOOTH);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // Pick a legal width (>0 and within range)
        // float wanted = 2.0f;                                                    // example
        // float legal = std::clamp(wanted, lineWidthRange[0], lineWidthRange[1]); // likely 1.0f
        // glLineWidth((GLfloat)2.0f); // legal);
        // avoid INVALID_VALUE

        GLsizei vertexCount = numLines * 2;
        glDrawArrays(GL_LINES, 0, vertexCount);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDisable(GL_BLEND);
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_POLYGON_OFFSET_LINE);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);
    }

    void DebugDrawRenderPass::updateNumLines(unsigned int nl) {
        numLines = nl;
    }

    void DebugDrawRenderPass::updateLineBuffer(std::shared_ptr<Graphics::GeometryBuffer> lb) {
        lineBuffer = lb;
    }

} // namespace LaurelEye::Graphics
