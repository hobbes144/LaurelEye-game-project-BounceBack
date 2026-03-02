#pragma once

#include "LaurelEyeEngine/graphics/resources/RenderState.h"

#include <glad/glad.h>

namespace LaurelEye::Graphics{

    struct LGLRenderState {
        // Current Bound Objects
        GLuint currentProgram = 0;
        GLuint currentVAO = 0;
        GLuint currentFBO = 0;

        // Pipeline State (for redundancy checking)
        bool depthTestEnabled = true;
        GLenum depthFunc = GL_LESS;
        bool blendingEnabled = false;
        GLenum blendEquation = GL_LESS;

        // Diagnostics
        uint64_t drawCallCount = 0;
    };

    GLenum blendEquationToGLBlendEquation(BlendEquation e);
    GLenum blendFactorToGLBlendFactor(BlendFactor f);
    GLenum depthFunctionToGLDepthFunction(DepthFunction d);

}
