/// @file   LGLRenderDevice.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of Render Device for OpenGL

#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderDevice.h"

#include "LaurelEyeEngine/graphics/device/glfw/LGLDataBufferFactory.h"
#include "LaurelEyeEngine/graphics/device/glfw/LGLFramebufferFactory.h"
#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderState.h"
#include "LaurelEyeEngine/graphics/device/glfw/LGLTextureFactory.h"
#include "LaurelEyeEngine/graphics/device/glfw/LGLVertexArrayFactory.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Framebuffer.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

#include <cstdint>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <memory>

namespace LaurelEye::Graphics {

    void openglCallbackFunction(
        GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam) {
        std::string sourceStr;
        switch ( source ) {
        case GL_DEBUG_SOURCE_API:
            sourceStr = "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            sourceStr = "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            sourceStr = "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            sourceStr = "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            sourceStr = "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            sourceStr = "Other";
            break;
        }

        std::string typeStr;
        switch ( type ) {
        case GL_DEBUG_TYPE_ERROR:
            typeStr = "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            typeStr = "Deprecated Behavior";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            typeStr = "Undefined Behavior";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            typeStr = "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            typeStr = "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            typeStr = "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            typeStr = "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            typeStr = "Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            typeStr = "Other";
            break;
        }

        std::string severityStr;
        switch ( severity ) {
        case GL_DEBUG_SEVERITY_HIGH:
            severityStr = "High";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            severityStr = "Medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            severityStr = "Low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            severityStr = "Notification";
            break;
        }

        std::cerr << "OpenGL Debug Message\n"
                  << "Source: " << sourceStr << "\n"
                  << "Type: " << typeStr << "\n"
                  << "ID: " << id << "\n"
                  << "Severity: " << severityStr << "\n"
                  << "Message: " << message << std::endl;
    }

    LGLRenderDevice::LGLRenderDevice() = default;
    LGLRenderDevice::~LGLRenderDevice() = default;

    void LGLRenderDevice::initialize() {
        if ( !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) ) {
            throw std::runtime_error("ERROR::RENDERSYSTEM::INIT::GLADINIT");
        }

        // Enabling debug output
#if !defined(NDEBUG)
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        dataBufferFactory = std::make_unique<LGLDataBufferFactory>();
        vertexArrayFactory = std::make_unique<LGLVertexArrayFactory>();
        textureFactory = std::make_unique<LGLTextureFactory>();
        framebufferFactory = std::make_unique<LGLFramebufferFactory>(textureFactory.get());
    }

    void LGLRenderDevice::shutdown() {
    }

    void LGLRenderDevice::clear() {
        glClear(clearMask);
    }

    /*!****************************************************************************
     * \brief Set the Viewport size
     *
     * ## Usage:
     *
     * This is to be called during RenderGraph Passes that require a custom
     * viewport size, such as for the ShadowPass.
     *
     * Ensure that viewport is correctly reset by using a RenderStateSaver.
     *
     * \param Viewport The Viewport struct storing the width and height.
     *****************************************************************************/
    void LGLRenderDevice::setViewport(const Viewport& viewport, bool force) {
        if ( !force && state.viewport == viewport ) return;

        glViewport(0, 0, viewport.size.width, viewport.size.height);
        state.viewport = viewport;
    }

    /*!****************************************************************************
     * \brief Set the Depth State
     *
     * ## Usage:
     *
     * This is to be called in RenderGraph Passes that require the Depth test to be
     * modified to a specific set of values.
     *
     * ## Note:
     *
     * clearMask is set here, which means that if depth testing is turned off
     * before running a clear, The depth buffer might still have values that never
     * get cleared until the next time depth testing is enabled and clear is
     * called.
     *
     * \param depthState DepthState object that stores the enabled and func.
     *****************************************************************************/
    void LGLRenderDevice::setDepthState(const DepthState& depthState, bool force) {
        if ( !force && state.depthState == depthState ) return;

        if ( depthState.testEnabled ) {
            glEnable(GL_DEPTH_TEST);
            clearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
            if ( depthState.writeEnabled )
                glDepthMask(GL_TRUE);
            else
                glDepthMask(GL_FALSE);
            glDepthFunc(depthFunctionToGLDepthFunction(depthState.func));
        }
        else {
            glDisable(GL_DEPTH_TEST);
            clearMask = GL_COLOR_BUFFER_BIT;
        }

        state.depthState = depthState;
    }

    /*!****************************************************************************
     * \brief Set the Blending State
     *
     * ## Usage:
     *
     * This is called in RenderGraph Passes to enable effects like transparency and
     * advanced lighting.
     *
     * \param blendState BlendState object that stores the enabled, equation and
     * func.
     *****************************************************************************/
    void LGLRenderDevice::setBlendState(const BlendState& blendState, bool force) {
        if ( !force && state.blendState == blendState ) return;

        if ( blendState.enabled ) {
            glEnable(GL_BLEND);
            glBlendEquation(blendEquationToGLBlendEquation(blendState.equation));
            glBlendFunc(blendFactorToGLBlendFactor(blendState.srcFactor), blendFactorToGLBlendFactor(blendState.destFactor));
        }
        else
            glDisable(GL_BLEND);

        state.blendState = blendState;
    }

    RenderState LGLRenderDevice::getCurrentState() {
        return state;
    }

    void LGLRenderDevice::setState(const RenderState& rs, bool force) {
        setViewport(state.viewport, force);
        setBlendState(state.blendState, force);
        setDepthState(state.depthState, force);
    }

    /// @brief Create Data buffer
    ///
    /// @param d Data Buffer description
    /// @param initialData Initial data (This must be the same as d.sizeBytes)
    /// @return DataBufferHandle
    DataBufferHandle LGLRenderDevice::createDataBuffer(const DataBufferDesc& d, const void* initialData, const std::string& debugName) {
        return dataBufferFactory->createBuffer(d, initialData, debugName);
    }

    void LGLRenderDevice::destroyDataBuffer(DataBufferHandle h) {
        dataBufferFactory->destroyBuffer(h);
    }

    void LGLRenderDevice::updateDataBufferSubData(DataBufferHandle h, size_t offset, size_t size, const void* data) {
        dataBufferFactory->updateSubData(h, offset, size, data);
    }

    void LGLRenderDevice::bindDataBufferBase(DataBufferHandle h) {
        dataBufferFactory->bindBase(h);
    }
    VertexArrayHandle LGLRenderDevice::createVertexArray(const VertexArrayDesc& d) {
        return vertexArrayFactory->createArray(d);
    }

    void LGLRenderDevice::destroyVertexArray(VertexArrayHandle h) {
        vertexArrayFactory->destroyArray(h);
    }

    void LGLRenderDevice::bindVertexArray(VertexArrayHandle h) {
        vertexArrayFactory->bindBase(h);
    }

    TextureHandle LGLRenderDevice::createTexture(const TextureDesc& d, const void* init) {
        return textureFactory->create(d, init);
    }

    void LGLRenderDevice::generateTextureMips(TextureHandle h) {
        textureFactory->generateMips(h);
    }

    void LGLRenderDevice::resizeTexture(TextureHandle h, uint32_t newW, uint32_t newH, uint32_t newD) {
        textureFactory->resize(h, newW, newH, newD);
    }

    void LGLRenderDevice::setTextureSampler(TextureHandle h, const SamplerDesc& sDesc) {
        textureFactory->setSampler(h, sDesc);
    }

    void LGLRenderDevice::destroyTexture(TextureHandle h) {
        textureFactory->destroy(h);
    }

    void LGLRenderDevice::destroyAllTextures() {
        textureFactory->destroyAll();
    }

    void LGLRenderDevice::bindTexture(TextureHandle h, uint32_t textureUnit) {
        textureFactory->bind(h, textureUnit);
    }

    FramebufferHandle LGLRenderDevice::createFramebuffer(const FramebufferDesc& d) {
        return framebufferFactory->create(d);
    }

    void LGLRenderDevice::finalizeFramebuffer(FramebufferHandle h) {
    }

    void LGLRenderDevice::destroyFramebuffer(FramebufferHandle h) {
        framebufferFactory->destroy(h);
    }

    void LGLRenderDevice::bindFramebufferBase(FramebufferHandle h) {
        framebufferFactory->bindBase(h);
    }

    uint32_t LGLRenderDevice::attachTexturetoFramebuffer(FramebufferHandle h, const FramebufferAttachmentDesc& d) {
        return framebufferFactory->attachTexture(h, d);
    }

    void LGLRenderDevice::resizeFramebuffer(FramebufferHandle h, uint32_t width, uint32_t height) {
        framebufferFactory->resize(h, width, height);
    }

    void LGLRenderDevice::blitFramebuffers(FramebufferHandle source, FramebufferHandle dest, bool depthOnly) {
        framebufferFactory->blit(source, dest, depthOnly);
    }

} // namespace LaurelEye::Graphics
