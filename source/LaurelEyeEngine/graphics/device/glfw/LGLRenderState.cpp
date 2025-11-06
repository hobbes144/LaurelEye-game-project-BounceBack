#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderState.h"
#include "LaurelEyeEngine/graphics/resources/RenderState.h"
#include <iostream>

namespace LaurelEye::Graphics {

    GLenum blendEquationToGLBlendEquation(BlendEquation e) {
        switch ( e ) {
        case BlendEquation::Add:
            return GL_FUNC_ADD;
        default:
            // TODO: Replace this with proper warning logging.
#if !defined(NDEBUG)
            std::cerr << "WARNING::RENDERSYSTEM::RENDER_STATE::INVALID_BLEND_EQUATION"
                      << "::Defaulting to GL_FUNC_ADD" << std::endl;
#endif
            return GL_FUNC_ADD;
        }
    }

    GLenum blendFactorToGLBlendFactor(BlendFactor f) {
        switch ( f ) {
        case BlendFactor::One:
            return GL_ONE;
        default:
            // TODO: Replace this with proper warning logging.
#if !defined(NDEBUG)
            std::cerr << "WARNING::RENDERSYSTEM::RENDER_STATE::INVALID_BLEND_FACTOR"
                      << "::Defaulting to GL_ONE" << std::endl;
#endif
            return GL_ONE;
        }
    }

    GLenum depthFunctionToGLDepthFunction(DepthFunction d) {
        switch ( d ) {
            case DepthFunction::Less:
            return GL_LESS;
        default:
            // TODO: Replace this with proper warning logging.
#if !defined(NDEBUG)
            std::cerr << "WARNING::RENDERSYSTEM::RENDER_STATE::INVALID_DEPTH_FUNCTION"
                      << "::Defaulting to GL_LESS" << std::endl;
#endif
            return GL_LESS;
        }
    }

} // namespace LaurelEye::Graphics
