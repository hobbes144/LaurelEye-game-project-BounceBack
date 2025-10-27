/// @file   RenderSystem.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of RenderSystem

#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/ecs/ISystem.h"
#include "LaurelEyeEngine/graphics/Graphics.h"

#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Lights.h"
#include "LaurelEyeEngine/graphics/surface/glfw/LGlfwOpenGLWindowSurface.h"

#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderDevice.h"

#include "LaurelEyeEngine/graphics/graphics_components/CameraComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/LightComponent.h"
#include "LaurelEyeEngine/graphics/renderpass/SingleBufferedDataPass.h"
#include "LaurelEyeEngine/graphics/renderpass/SinglePass.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"

#include <memory>
#include <stdexcept>

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
        // sp = std::make_shared<SinglePass>();
        sp = std::make_shared<SingleBufferedDataPass>();
        sp->setup(*tempRenderResources.get());

        initDefaultCamera();
        initGlobalLightsBuffer();
    }

    void RenderSystem::update(float deltaTime) {
        // TODO: All the draw logic should run here.

        windowSurfaces[0]->resizeSurfaceCallback();

        // Camera update
        updateCameraBuffer(camera);
        // for ( size_t i = 0; i < GetCameraComponents().size(); i++ ) {
        //     GetCameraComponents()[i]->updateViewMatrix();
        //     if ( GetCameraComponents()[i]->getInitStatus() == false ) {
        //         initCameraBuffer(GetCameraComponents()[i]);
        //     }
        //     else {
        //         updateCameraBuffer(GetCameraComponents()[i]);
        //     }
        // }

        // Light update
        // TODO: Eventually remove this command from here. Ideally we don't
        // need to keep calling this. We just call it when there's a change.
        updateGlobalLights();

        // TODO: Code for point lights: eventually move to local lights
        // interaction functions.
        //
        // for ( size_t i = 0; i < GetLightComponents().size(); i++ ) {
        //     if ( GetLightComponents()[i]->getInitStatus() == false && GetLightComponents()[i]->getType() == LightType::Point ) {
        //         pointLight = dynamic_cast<PointLightComponent*>(GetLightComponents()[i]);
        //         pointLight->setHandle(device->createDataBuffer(DataBufferDesc{DataBufferType::SSBO, DataBufferUpdateMode::Dynamic, sizeof(PointLight), 2}));
        //
        //         pointLight->setInitStatus(true);
        //     }
        //     else {
        //         if ( GetLightComponents()[i]->getType() == LightType::Point ) {
        //             pointLight = dynamic_cast<PointLightComponent*>(GetLightComponents()[i]);
        //             pointLight->getLightData();
        //             device->bindDataBufferBase(pointLight->getHandle());
        //             device->updateDataBufferSubData(
        //                 pointLight->getHandle(),
        //                 0,
        //                 sizeof(PointLight),
        //                 &(pointLight->getLightData()));
        //         }
        //     }
        // }

        // Bind camera and global lights buffer for use in shaders
        device->bindDataBufferBase(camera->getCameraBufferHandle());
        device->bindDataBufferBase(globalLightsBufferHandle);

        // In future, this needs to be swapped to per window, but this is more than good enough for now.
        windowSurfaces[0]->beginFrame();

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
            assert(dynamic_cast<CameraComponent*>(component) && "ERROR::GRAPHICS::RENDERSYSTEM::INVALID_LIGHT");
            // TODO: This is currently updating camera to the new component
            // each time. This should be changed when we handle multiple
            // cameras.
            if (defaultCamera) {
                defaultCamera = nullptr;
            }
            camera = static_cast<CameraComponent*>(component);
            assert(camera && "ERROR::GRAPHICS::RENDERSYSTEM::INVALID_CAMERA");
            cameraProperties[camera->GetRenderID()] = camera;
            initCameraBuffer(camera);
            updateCameraBuffer(camera);
            break;
        }
        case RenderComponentType::PropertyLight: {
            assert(dynamic_cast<LightComponent*>(component) && "ERROR::GRAPHICS::RENDERSYSTEM::INVALID_LIGHT");
            auto* light = static_cast<LightComponent*>(component);
            lightProperties[light->GetRenderID()] = light;
            registerLight(light);
            break;
        }
        default:
            break;
        }

        component->BindTransform();
        component->rs = this;
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
                if (it->second->getInitStatus())
                    destroyCameraBuffer(it->second);
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

    // Camera interactons

    // TODO: Delete this when the full scenegraph is up, we don't want to do this.
    void RenderSystem::initDefaultCamera() {
        defaultCamera = std::make_unique<Entity>("Default Camera");
        auto defaultCameraT = defaultCamera->addComponent<TransformComponent>();
        auto defaultCameraComponent = defaultCamera->addComponent<CameraComponent>();
        defaultCameraComponent->setPerspectiveProjection(45.0f * 3.14159f / 180.0f, 1280.f / 720.f, 0.1f, 1000.0f);
        camera = defaultCameraComponent;
        cameraProperties[defaultCameraComponent->GetRenderID()] = defaultCameraComponent;
        initCameraBuffer(defaultCameraComponent);
        updateCameraBuffer(defaultCameraComponent);

        defaultCameraComponent->BindTransform();
        defaultCameraComponent->rs = this;
    }

    void RenderSystem::initCameraBuffer(CameraComponent* camera) {
        // TODO: Update code to handle multiple cameras.
        camera->setCameraBufferHandle(device->createDataBuffer(DataBufferDesc{
            DataBufferType::UBO, DataBufferUpdateMode::Dynamic,
            sizeof(Camera),
            0}));
        camera->setInitStatus(true);
    }

    void RenderSystem::updateCameraBuffer(CameraComponent* camera) {
        assert(camera && "ERROR::GRAPHICS::RENDERSYSTEM::CAMERA_UNINITIALIZED");
        device->updateDataBufferSubData(camera->getCameraBufferHandle(), 0, sizeof(Camera), camera->getCameraDataPtr());
    }

    void RenderSystem::destroyCameraBuffer(CameraComponent* camera) {
        device->destroyDataBuffer(camera->getCameraBufferHandle());
        camera->setInitStatus(false);
    }

    // Light interactions
    void RenderSystem::registerLight(LightComponent* light) {
        switch ( light->getType() ) {
        case LightType::Ambient:
            globalLights.ambientLight = static_cast<AmbientLightComponent*>(light)->getLightData();
            updateGlobalLights();
            break;
        case LightType::Directional:
            globalLights.sunLight = static_cast<DirectionalLightComponent*>(light)->getLightData();
            updateGlobalLights();
            break;
        case LightType::Point:
            // registerLocalLight(light);
            break;
        case LightType::Default:
        default:
            throw std::logic_error("ERROR::GRAPHICS::RENDERSYSTEM::INVALID_LIGHT_TYPE");
            break;
        }

        light->setHandle(globalLightsBufferHandle);
    }

    void RenderSystem::initGlobalLightsBuffer() {
        globalLightsBufferHandle = device->createDataBuffer(
            DataBufferDesc{
                DataBufferType::UBO,
                DataBufferUpdateMode::Dynamic,
                sizeof(GlobalLights),
                1},
            &globalLights);
    }

    void RenderSystem::updateGlobalLights() const {
        assert(isValid(globalLightsBufferHandle) && "ERROR::GRAPHICS::RENDERSYSTEM::GLOBALLIGHTS_UNINITIALIZED");
        device->updateDataBufferSubData(
            globalLightsBufferHandle, 0,
            sizeof(GlobalLights), &globalLights);
    }

} // namespace LaurelEye::Graphics
