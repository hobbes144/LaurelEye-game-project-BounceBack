#pragma once

#include "LaurelEyeEngine/graphics/resources/RenderState.h"

#include <glad/glad.h>

namespace LaurelEye::Graphics{

    GLenum blendEquationToGLBlendEquation(BlendEquation e);
    GLenum blendFactorToGLBlendFactor(BlendFactor f);
    GLenum depthFunctionToGLDepthFunction(DepthFunction d);

}
