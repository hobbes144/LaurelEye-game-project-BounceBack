/// @file   RenderSystem.h
/// @author Anish Murthy
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date   10-08-2025
/// @brief  Core rendering system responsible for managing render components,
///         graphics backend initialization, and per-frame rendering operations.
///
/// The RenderSystem acts as the bridge between the ECS layer and the underlying
/// rendering backend (e.g., OpenGL, Vulkan, DirectX). It is responsible for
/// managing renderable components such as cameras and lights, initializing
/// device resources, and orchestrating frame rendering.

#pragma once

#include "LaurelEyeEngine/ecs/ISystem.h"
#include "LaurelEyeEngine/graphics/Graphics.h"
#include "LaurelEyeEngine/graphics/graphics_components/IRenderComponent.h"
#include "LaurelEyeEngine/graphics/ShadowManager.h"

// Temp
#include "LaurelEyeEngine/graphics/resources/Lights.h"
#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"
#include "LaurelEyeEngine/graphics/surface/IWindowSurfaceProvider.h"

#include <functional>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace LaurelEye {
    class IWindow;
}

namespace LaurelEye::Graphics {

    class IRenderDevice;
    class MeshManager;
    class SkeletonManager;

    // temp
    class LightComponent;
    class CameraComponent;
    class GBufferPass;
    class GBufferSkinnedPass;
    class DeferredRenderPass;
    class LocalLightsToonPass;
    class SinglePass;
    class SingleBufferedDataPass;
    class SinglePassShadow;
    class ParticleRenderPass;
    class DebugDrawRenderPass;
    class SkydomePass;
    class UIPass;

    /// @struct RenderSystemConfig
    /// @brief Configuration data for the Render System
    ///
    /// The configuration defines which graphics backend to use and specifies
    /// the windows that the system will render to.
    struct RenderSystemConfig {
        GraphicsBackend backend = GraphicsBackend::OpenGL;
        std::vector<IWindow*> windows;
        Vector4 clearColor = Vector4(0.2f, 0.3f, 0.3f, 1.0f);
    };

    /// @class RenderSystem
    /// @brief Main rendering system that integrates ECS render components with the GPU backend.
    ///
    /// The `RenderSystem` manages all active render components (e.g., cameras and lights),
    /// controls the rendering device, and handles frame-level operations such as resource
    /// updates and render pass execution.
    ///
    /// Responsibilities include:
    /// - Initializing and shutting down the graphics backend.
    /// - Managing the lifecycle of renderable entities and property components.
    /// - Coordinating render passes and GPU resource updates.
    class RenderSystem final : public ISystem<IRenderComponent> {
    public:
        /// @brief Constructs a new RenderSystem with an optional backend specification.
        /// @param backend The graphics backend to use (default: OpenGL).
        RenderSystem(GraphicsBackend backend = GraphicsBackend::OpenGL);
        /// @brief Destructor. Releases GPU resources and shuts down the backend.
        ~RenderSystem();

        /// \copydoc ISystem::initialize()
        void initialize() override;
        /// \copydoc ISystem::update
        void update(float deltaTime) override;
        /// \copydoc ISystem::shutdown
        void shutdown() override;

        /// \copydoc ISystem::registerComponent
        void registerComponent(const ComponentPtr component) override;

        /// \copydoc ISystem::deregisterComponent
        void deregisterComponent(const ComponentPtr component) override;

        /// @brief Applies configuration settings to the Render System.
        /// @param config The configuration containing backend and window parameters.
        void setConfig(const RenderSystemConfig& config);

        /// @brief Returns a reference to all registered light components.
        /// @return Unordered map of light IDs to their corresponding components.
        const std::unordered_map<unsigned int, LightComponent*>& GetLightProperties() const;

        /// @brief Returns a reference to all registered camera components.
        /// @return Unordered map of camera IDs to their corresponding components.
        const std::unordered_map<unsigned int, CameraComponent*>& GetCameraProperties() const;

        /// @brief Retrieves all unique light component IDs.
        /// @return A vector containing the render IDs of all light components.
        std::vector<unsigned int> GetLightIDs() const;

        /// @brief Retrieves all unique camera component IDs.
        /// @return A vector containing the render IDs of all camera components.
        std::vector<unsigned int> GetCameraIDs() const;

        /// @brief Retrieves all registered light component pointers.
        /// @return A vector containing pointers to all light components.
        std::vector<LightComponent*> GetLightComponents() const;

        /// @brief Retrieves all registered camera component pointers.
        /// @return A vector containing pointers to all camera components.
        std::vector<CameraComponent*> GetCameraComponents() const;

        void setCurrentCamera(CameraComponent* currCamera);

        std::shared_ptr<SinglePassShadow> retrieveSinglePass();
        std::shared_ptr<ParticleRenderPass> retrieveParticlePass();
        std::shared_ptr<SkydomePass> retrieveSkydomePass();
        // std::shared_ptr<GBufferPass> retrieveGBufferPass();
        std::shared_ptr<GBufferSkinnedPass> retrieveGBufferPass();
        std::shared_ptr<DebugDrawRenderPass> retrieveDebugDrawRenderPass();

        RenderResources* getRenderResources() {
            return tempRenderResources.get();
        }

        MeshManager* getMeshManager() {
            return meshManager.get();
        }

        SkeletonManager* getSkeletonManager() {
            return skeletonManager.get();
        }

        void setClearColor(float r, float g, float b, float a = 1.0f) {
            config.clearColor = Vector4(r, g, b, a);
        }

        void setRunDebugDraw(bool r) {
            runDebugDraw = r;
        }

        bool testParticles = true;

        void registerResizeCallback(SurfaceHandle h, std::function<void(const SizeRegistry&)> callback);

    private:
        /// @brief Configuration data used to initialize the render system.
        RenderSystemConfig config;
        /// @brief Rendering device interface responsible for low-level GPU communication.
        std::unique_ptr<IRenderDevice> device;

        bool runDebugDraw = false;

        /// @brief List of window surfaces associated with each window instance.
        std::vector<std::unique_ptr<IWindowSurfaceProvider>> windowSurfaces;

        // TODO: potential optimization, merge into 1 because theye all have unique ID's

        /// @brief Mapping of unique light component IDs to their instances.
        std::unordered_map<unsigned int, LightComponent*> lightProperties;
        /// @brief Mapping of unique camera component IDs to their instances.
        std::unordered_map<unsigned int, CameraComponent*> cameraProperties;
        /// @brief List of UI render components registered with the system.
        std::vector<ComponentPtr> uiComponents;

        /// @brief Initializes the chosen graphics backend and its associated device.
        void graphicsBackendInit();
        /// @brief Creates surface providers for each configured window.
        void createWindowSurfaces();

        void resize(SurfaceHandle h, const SizeRegistry& size);

        std::map<SurfaceHandle, std::vector<std::function<void(const SizeRegistry&)>>> resizeCallbacks;

        // NOTE: The Mesh Manager is closely tied to the RS being initialized
        // So this might actually be final.

        /// @bried Mesh Manager
        std::unique_ptr<MeshManager> meshManager;

        /// @brief Skeleton Manager
        std::unique_ptr<SkeletonManager> skeletonManager;

        /// @brief Temporary rendering resource storage used during frame execution.
        std::unique_ptr<RenderResources> tempRenderResources;
        std::unique_ptr<ShadowManager> tempShadowManager;
        /// @brief A single-pass rendering pipeline used for simple frame rendering.
        // std::shared_ptr<SinglePass> sp;
        // std::shared_ptr<GBufferPass> gbufferPass;
        std::shared_ptr<GBufferSkinnedPass> gbufferPass;
        std::shared_ptr<DeferredRenderPass> deferredRenderPass;
        std::shared_ptr<LocalLightsToonPass> localLightsPass;
        std::shared_ptr<SinglePassShadow> sp;
        std::shared_ptr<SkydomePass> bp;
        // std::shared_ptr<SingleBufferedDataPass> sp;
        std::shared_ptr<UIPass> uiPass;
        std::shared_ptr<ParticleRenderPass> prp;
        std::shared_ptr<DebugDrawRenderPass> dbrp;

        // Temp pointers to support lights and camera updates
        std::unique_ptr<Entity> defaultCamera = nullptr;
        CameraComponent* camera = nullptr;

        // Lighting pass lights
        DataBufferHandle globalLightsBufferHandle = DataBuffer::InvalidDataBuffer;
        // TODO: Update this to correctly handle global and local lights.
        GlobalLights globalLights;
        bool globalLightsInitStatus = false;
        DataBufferHandle localLightsBufferHandle = DataBuffer::InvalidDataBuffer;
        // TODO: Update this to correctly handle global and local lights.
        LocalLights localLights;
        bool localLightsInitStatus = false;

        std::unordered_map<unsigned int, size_t> pointLightMapping;

        // Camera interactions
        void initDefaultCamera();
        void initCameraBuffer(CameraComponent* camera);
        void updateCameraBuffer(CameraComponent* camera);
        void destroyCameraBuffer(CameraComponent* camera);

        // Light interactions
        void registerLight(LightComponent* light);
        void deregisterLight(LightComponent* light);
        void initGlobalLightsBuffer();
        void initLocalLightsBuffer();
        void updateGlobalLights() const;
        // void updateLocalLight(LightComponent* light);
        void updateLocalLights();
    };
} // namespace LaurelEye::Graphics
