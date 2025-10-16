/// @file   LGLRenderDevice.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of Render Device for OpenGL

#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderDevice.h"

#include "LaurelEyeEngine/graphics/device/glfw/LGLDataBufferFactory.h"
#include "LaurelEyeEngine/graphics/device/glfw/LGLTextureFactory.h"
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

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

        // Enabling debug output for now
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        glEnable(GL_DEBUG_OUTPUT);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        dataBufferFactory = std::make_unique<LGLDataBufferFactory>();
        textureFactory = std::make_unique<LGLTextureFactory>();
    }

    void LGLRenderDevice::shutdown() {
        // dataBufferFactory->destroyAll();
        textureFactory->destroyAll();
    }

    /// @brief Create Data buffer
    ///
    /// @param d Data Buffer description
    /// @param initialData Initial data (This must be the same as d.sizeBytes)
    /// @return DataBufferHandle
    DataBufferHandle LGLRenderDevice::createDataBuffer(const DataBufferDesc& d, const void* initialData) {
        return dataBufferFactory->createBuffer(d, initialData);
    }

    void LGLRenderDevice::destroyDataBuffer(DataBufferHandle h) {
        dataBufferFactory->destroyBuffer(h);
    }

    void LGLRenderDevice::updateDataBufferSubData(DataBufferHandle h, uint64_t offset, uint64_t size, const void* data) {
        dataBufferFactory->updateSubData(h, offset, size, data);
    }

    void LGLRenderDevice::bindDataBufferBase(DataBufferHandle h) {
        dataBufferFactory->bindBase(h);
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

} // namespace LaurelEye::Graphics
