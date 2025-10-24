/// @file   RenderSystem.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of RenderSystem

#include "LaurelEyeEngine/ecs/ISystem.h"
#include "LaurelEyeEngine/graphics/Graphics.h"

#include "LaurelEyeEngine/graphics/surface/glfw/LGlfwOpenGLWindowSurface.h"
#include "LaurelEyeEngine/window/IWindow.h"

#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderDevice.h"

#include "LaurelEyeEngine/graphics/graphics_components/CameraComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/IRenderableComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/LightComponent.h"
#include "LaurelEyeEngine/graphics/renderpass/SinglePass.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"

#include <memory>

namespace LaurelEye::Graphics {

    RenderSystem::RenderSystem(LaurelEye::Graphics::GraphicsBackend backend)
        : LaurelEye::ISystem<IRenderComponent>() {}

    RenderSystem::~RenderSystem() = default;

    void RenderSystem::setConfig(const RenderSystemConfig& config) {
        this->config = config;
    }

    void RenderSystem::graphicsBackendInit() {
        if ( config.backend == GraphicsBackend::OpenGL ) {
            device = std::make_unique<LGLRenderDevice>();
            device->initialize();
        }
    }

    void RenderSystem::createWindowSurfaces() {
        if ( config.backend == GraphicsBackend::OpenGL ) {
            for ( int i = 0; i < config.windows.size(); ++i ) {
                std::unique_ptr<LGlfwOpenGLWindowSurface> windowSurface = std::make_unique<LGlfwOpenGLWindowSurface>();
                windowSurface->attachToWindow(*(config.windows[i]));

                windowSurfaces.emplace_back(std::move(windowSurface));
            }
        }
    }

    void RenderSystem::initialize() {
        graphicsBackendInit();
        createWindowSurfaces();

        // Temp code starts here:
        tempRenderResources = std::make_unique<RenderResources>(*device.get());
        sp = std::make_shared<SinglePass>();
        sp->setup(*tempRenderResources.get());
    }

    void RenderSystem::update(float deltaTime) {
        // TODO: All the draw logic should run here.

        // Camera update
        for ( size_t i = 0; i < GetCameraComponents().size(); i++ ) {
            GetCameraComponents()[i]->updateViewMatrix();
            if ( GetCameraComponents()[i]->getInitStatus() == false ) {
                GetCameraComponents()[i]->setCameraBufferHandle(device->createDataBuffer(DataBufferDesc{
                    DataBufferType::UBO, DataBufferUpdateMode::Dynamic,
                    (sizeof(Matrix4) * 3) + sizeof(float),
                    0}));

                GetCameraComponents()[i]->setInitStatus(true);
            }
            else {
                device->updateDataBufferSubData(GetCameraComponents()[i]->getCameraBufferHandle(), 0, sizeof(Matrix4), GetCameraComponents()[i]->getProjectionMatrix().getData());
                device->updateDataBufferSubData(GetCameraComponents()[i]->getCameraBufferHandle(), sizeof(Matrix4), sizeof(Matrix4), GetCameraComponents()[i]->getViewMatrix().getData());
                device->updateDataBufferSubData(GetCameraComponents()[i]->getCameraBufferHandle(), sizeof(Matrix4) * 2, sizeof(Matrix4), GetCameraComponents()[i]->getInverseViewMatrix().getData());
                device->updateDataBufferSubData(GetCameraComponents()[i]->getCameraBufferHandle(), sizeof(Matrix4) * 3, sizeof(float), &(GetCameraComponents()[i]->getExposure()));
            }
        }

        // Light update
        for ( size_t i = 0; i < GetLightComponents().size(); i++ ) {
            if ( GetLightComponents()[i]->getInitStatus() == false && GetLightComponents()[i]->getType() == LightType::Point ) {
                pointLight = dynamic_cast<PointLightComponent*>(GetLightComponents()[i]);
                pointLight->setHandle(device->createDataBuffer(DataBufferDesc{DataBufferType::SSBO, DataBufferUpdateMode::Dynamic, sizeof(PointLight), 2}));

                pointLight->setInitStatus(true);
            }
            else {
                if ( GetLightComponents()[i]->getType() == LightType::Point ) {
                    pointLight = dynamic_cast<PointLightComponent*>(GetLightComponents()[i]);
                    pointLight->getLightData();
                    device->bindDataBufferBase(pointLight->getHandle());
                    device->updateDataBufferSubData(
                        pointLight->getHandle(),
                        0,
                        sizeof(PointLight),
                        &(pointLight->getLightData()));
                }
                else if ( GetLightComponents()[i]->getType() == LightType::Directional ) {
                    dirLight = dynamic_cast<DirectionalLightComponent*>(GetLightComponents()[i]);
                    lightingPassLights.sunLight = dirLight->getLightData();
                }
                else if ( GetLightComponents()[i]->getType() == LightType::Ambient ) {
                    ambLight = dynamic_cast<AmbientLightComponent*>(GetLightComponents()[i]);
                    lightingPassLights.ambientLight = ambLight->getLightData();
                }
            }
        }

        if ( !lightingPassLightsInitStatus ) {
            lightingPassLightsBufferHandle = device->createDataBuffer(DataBufferDesc{DataBufferType::UBO, DataBufferUpdateMode::Dynamic, sizeof(LocalLights), 1}, &lightingPassLights);
            lightingPassLightsInitStatus = true;
        }
        else {
            device->bindDataBufferBase(lightingPassLightsBufferHandle);
            device->updateDataBufferSubData(lightingPassLightsBufferHandle, 0, sizeof(LocalLights), &lightingPassLights);
        }

        device->updateDataBufferSubData(lightingPassLightsBufferHandle, 0, sizeof(LocalLights), &lightingPassLights);

        // In future, this needs to be swapped to per window, but this is more than good enough for now.
        windowSurfaces[0]->beginFrame();

        // tempShader->use();
        // tempShader->setUInt("height", config.windows[0]->getHeight());
        // tempShader->setUInt("width", config.windows[0]->getWidth());

        // Draw calls here.
        // for ( const auto& transform : tempTransforms ) {
        // tempShader->setMat4("ModelMatrix", transform.getLocalMatrix());
        // tempMesh->draw(GL_TRIANGLES);
        // }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // tempShader->unuse();
        FrameContext ctx{
            0.1f,
            *device.get(),
            *tempRenderResources.get(),
            components};

        sp->execute(ctx);

        windowSurfaces[0]->endFrame();
    }

    void RenderSystem::shutdown() {
        // tempMesh = nullptr;
        // tempShader = nullptr;
        sp = nullptr;
        ShaderManager::getInstance().unloadShaders();
    }

    void RenderSystem::registerComponent(const ComponentPtr component) {

        switch ( component->GetRenderCompType() ) {
        case RenderComponentType::Renderable3D:
            ISystem::registerComponent(component);
            break;
        case RenderComponentType::Renderable2D:
            ISystem::registerComponent(component);
            break;
        case RenderComponentType::PropertyCamera: {
            auto* camera = dynamic_cast<CameraComponent*>(component);
            if ( camera )
                cameraProperties[camera->GetRenderID()] = camera;
            else
                std::cerr << "Error: component is not a CameraComponent!\n";
            break;
        }
        case RenderComponentType::PropertyLight: {
            auto* light = dynamic_cast<LightComponent*>(component);
            if ( light )
                lightProperties[light->GetRenderID()] = light;
            else
                std::cerr << "Error: component is not a LightComponent!\n";
            break;
        }
        default:
            break;
        }

        component->BindTransform();
    }

    void RenderSystem::deregisterComponent(const ComponentPtr component) {
        switch ( component->GetRenderCompType() ) {
        case RenderComponentType::Renderable3D:
            ISystem::deregisterComponent(component);
            break;
        case RenderComponentType::Renderable2D:
            ISystem::deregisterComponent(component);
            break;
        case RenderComponentType::PropertyCamera: {
            auto it = cameraProperties.find(component->GetRenderID());
            if ( it != cameraProperties.end() ) {
                cameraProperties.erase(it);
            }
            break;
        }
        case RenderComponentType::PropertyLight: {
            auto it = lightProperties.find(component->GetRenderID());
            if ( it != lightProperties.end() ) {
                lightProperties.erase(it);
            }
            break;
        }
        default:
            break;
        }
    }

    /*!****************************************************************************
     * \brief Draw objects in buffers
     *
     * ## Usage:
     *
     * Calls the appropriate draw call to draw the objects in the buffers.
     *
     * \param mode OpenGL Draw mode (eg. GL_TRIANGLES)
     * \param count Number of vertices to be drawn
     * \param indexed If true, EBO will be used. Defaults to `true`.
     *****************************************************************************/
    // void RenderSystem::tempDraw(GLenum mode, GLint count, bool indexed) const {
    //     if ( indexed ) {
    //         glDrawElements(mode, count, GL_UNSIGNED_INT, 0);
    //     }
    //     else {
    //         glDrawArrays(mode, 0, count);
    //     }
    // }
    //
    // void RenderSystem::setTransforms(std::vector<Transform> transforms) {
    //     tempTransforms = transforms;
    // }

    const std::unordered_map<unsigned int, LightComponent*>& RenderSystem::GetLightProperties() const {
        return lightProperties;
    }

    const std::unordered_map<unsigned int, CameraComponent*>& RenderSystem::GetCameraProperties() const {
        return cameraProperties;
    }

    // --- KEY ACCESSORS ---
    std::vector<unsigned int> RenderSystem::GetLightIDs() const {
        std::vector<unsigned int> keys;
        keys.reserve(lightProperties.size());
        for ( const auto& pair : lightProperties )
            keys.push_back(pair.first);
        return keys;
    }

    std::vector<unsigned int> RenderSystem::GetCameraIDs() const {
        std::vector<unsigned int> keys;
        keys.reserve(cameraProperties.size());
        for ( const auto& pair : cameraProperties )
            keys.push_back(pair.first);
        return keys;
    }

    // --- VALUE ACCESSORS ---
    std::vector<LightComponent*> RenderSystem::GetLightComponents() const {
        std::vector<LightComponent*> values;
        values.reserve(lightProperties.size());
        for ( const auto& pair : lightProperties )
            values.push_back(pair.second);
        return values;
    }

    std::vector<CameraComponent*> RenderSystem::GetCameraComponents() const {
        std::vector<CameraComponent*> values;
        values.reserve(cameraProperties.size());
        for ( const auto& pair : cameraProperties )
            values.push_back(pair.second);
        return values;
    }

} // namespace LaurelEye::Graphics
