#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderState.h"
#include "LaurelEyeEngine/graphics/resources/RenderState.h"
#include "LaurelEyeEngine/logging/EngineLog.h"

namespace LaurelEye::Graphics {

    GLenum blendEquationToGLBlendEquation(BlendEquation e) {
        switch ( e ) {
        case BlendEquation::Add:
            return GL_FUNC_ADD;
        default:
            LE_WARN("RenderSystem", "RenderState: Invalid blend equation. Defaulting to GL_FUNC_ADD");
            return GL_FUNC_ADD;
        }
    }

    GLenum blendFactorToGLBlendFactor(BlendFactor f) {
        switch ( f ) {
        case BlendFactor::One:
            return GL_ONE;
        default:
            LE_WARN("RenderSystem", "RenderState: Invalid blend factor. Defaulting to GL_ONE");
            return GL_ONE;
        }
    }

    GLenum depthFunctionToGLDepthFunction(DepthFunction d) {
        switch ( d ) {
            case DepthFunction::Less:
            return GL_LESS;
        default:
            LE_WARN("RenderSystem", "RenderState: Invalid depth function. Defaulting to GL_LESS");
            return GL_LESS;
        }
    }

} // namespace LaurelEye::Graphics
