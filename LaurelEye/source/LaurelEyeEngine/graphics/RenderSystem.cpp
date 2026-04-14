/// @file   RenderSystem.cpp
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    10-14-2025
/// @brief Implementation of RenderSystem

#include "LaurelEyeEngine/graphics/RenderSystem.h"

// System headers
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/ecs/ISystem.h"

// Device headers
#include "LaurelEyeEngine/graphics/device/glfw/LGLRenderDevice.h"
#include "LaurelEyeEngine/graphics/Graphics.h"

// Utility headers
#include "LaurelEyeEngine/graphics/RenderStateSaver.h"

// Surface headers
#include "LaurelEyeEngine/graphics/renderpass/GBufferSkinnedPass.h"
#include "LaurelEyeEngine/graphics/graphics_components/PointLightComponent.h"
#include "LaurelEyeEngine/graphics/renderpass/LocalLightsPass.h"
#include "LaurelEyeEngine/graphics/surface/glfw/LGlfwOpenGLWindowSurface.h"
#include "LaurelEyeEngine/graphics/surface/IWindowSurfaceProvider.h"

// Manager headers
#include "LaurelEyeEngine/graphics/MeshManager.h"
#include "LaurelEyeEngine/graphics/ShaderManager.h"
#include "LaurelEyeEngine/graphics/ShadowManager.h"
#include "LaurelEyeEngine/graphics/SkeletonManager.h"
#include "LaurelEyeEngine/graphics/FontManager.h"

// Resource headers
#include "LaurelEyeEngine/graphics/resources/DataBuffer.h"
#include "LaurelEyeEngine/graphics/resources/Font.h"
#include "LaurelEyeEngine/graphics/resources/FrameContext.h"
#include "LaurelEyeEngine/graphics/resources/Lights.h"
#include "LaurelEyeEngine/graphics/resources/RenderMesh.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/graphics/resources/Shadow.h"
#include "LaurelEyeEngine/graphics/resources/SizeRegistry.h"
#include "LaurelEyeEngine/io/Assets.h"

// Component headers
#include "LaurelEyeEngine/graphics/graphics_components/AmbientLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/CameraComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/DirectionalLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/LightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/Renderable3DComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UIRenderComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UITextComponent.h"

// Render pass Headers
#include "LaurelEyeEngine/graphics/renderpass/DebugDrawRenderPass.h"
#include "LaurelEyeEngine/graphics/renderpass/DeferredRenderPass.h"
#include "LaurelEyeEngine/graphics/renderpass/ParticleRenderPass.h"
#include "LaurelEyeEngine/graphics/renderpass/SinglePass.h"
#include "LaurelEyeEngine/graphics/renderpass/SinglePassShadow.h"
#include "LaurelEyeEngine/graphics/renderpass/SkydomePass.h"
#include "LaurelEyeEngine/graphics/renderpass/UIPass.h"
// #include "LaurelEyeEngine/graphics/renderpass/SingleBufferedDataPass.h"
#include "LaurelEyeEngine/graphics/renderpass/LocalLightsToonPass.h"

#include "LaurelEyeEngine/logging/EngineLog.h"


#include <memory>
#include <algorithm>
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
                windowSurface->setRenderSystemResizeCallback([this](SurfaceHandle h, const SizeRegistry& size) {
                    this->resize(h, size);
                });

                windowSurfaces.emplace_back(std::move(windowSurface));
            }
        }
    }

    void RenderSystem::registerResizeCallback(SurfaceHandle h, std::function<void(const SizeRegistry&)> callback) {
        resizeCallbacks[h].push_back(callback);
    }

    void RenderSystem::resize(SurfaceHandle h, const SizeRegistry& size) {
        // Update cameras to use new window size
        // When we add multiple cameras, we can change this to a `for` loop.
        camera->updateAspectRatio((float)size.width / (float)size.height);

        // Propogate the resize to resources that requested a callback.
        for ( const auto& callback : resizeCallbacks[h] ) {
            callback(size);
        }
    }

    void RenderSystem::initialize() {
        LE_ASSERT("Graphics", CHAR_BIT * sizeof(float) == 32, "How did we get here? FLOAT_NOT_32");

        graphicsBackendInit();
        createWindowSurfaces();

        // Temp code starts here:
        tempRenderResources = std::make_unique<RenderResources>(*device.get(), this);
        for ( const auto& windowSurface : windowSurfaces ) {
            // NOTE: This logic has not at all been set up, so hardcoding.
            tempRenderResources->setSurfaceSize(0, windowSurface->getSize());
        }

        // gbufferPass = std::make_shared<GBufferPass>();
        gbufferPass = std::make_shared<GBufferSkinnedPass>();
        gbufferPass->setup(*tempRenderResources.get());

        deferredRenderPass = std::make_shared<DeferredRenderPass>();
        deferredRenderPass->setup(*tempRenderResources.get());

        localLightsPass = std::make_shared<LocalLightsToonPass>();
        localLightsPass->setup(*tempRenderResources.get());

        // sp = std::make_shared<SinglePass>();
        // sp = std::make_shared<SinglePassShadow>();
        // sp = std::make_shared<SingleBufferedDataPass>();
        // sp->setup(*tempRenderResources.get());

        bp = std::make_shared<SkydomePass>();
        bp->setup(*tempRenderResources.get());

        uiPass = std::make_shared<UIPass>();
        uiPass->setup(*tempRenderResources.get());

        if ( testParticles ) {
            prp = std::make_shared<ParticleRenderPass>();
            prp->setup(*tempRenderResources.get());
        }

        if ( runDebugDraw ) {
            dbrp = std::make_shared<DebugDrawRenderPass>();
            dbrp->setup(*tempRenderResources.get());
        }

        initDefaultCamera();
        initGlobalLightsBuffer();
        initLocalLightsBuffer();

        tempShadowManager = std::make_unique<ShadowManager>(tempRenderResources.get());
        FontManager::getInstance().initialize(tempRenderResources.get());

        skeletonManager = std::make_unique<SkeletonManager>();
        meshManager = std::make_unique<MeshManager>(*tempRenderResources.get(), *skeletonManager.get());
    }

    void RenderSystem::setCurrentCamera(CameraComponent* currCamera) {
        camera = currCamera;
    }

    void RenderSystem::update(float deltaTime) {
        // TODO: All the draw logic should run here.
        // Camera update
        if ( camera ) {
            if ( TransformComponent* bound = camera->GetBoundTransform() ) {
                camera->updateFromTransform(bound);
            }
            updateCameraBuffer(camera);
            device->bindDataBufferBase(camera->getCameraBufferHandle());
        }
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
        device->bindDataBufferBase(globalLightsBufferHandle);

        updateLocalLights();
        device->bindDataBufferBase(localLightsBufferHandle);

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

        // FrameContext for all shadow casting objects in scene.
        // Typically this should not include all the non-shadow casting
        // component objects.
        FrameContext shadowCtx{
            0.1f,
            *device.get(),
            *tempRenderResources.get(),
            components,
        };
        {
            RenderStateSaver test = RenderStateSaver(device.get());
            tempShadowManager->update(shadowCtx, &globalLights, &localLights);
            tempShadowManager->bindShadowBuffer();
        }

        // FrameContext for all objects in scene.
        FrameContext ctx{
            0.1f,
            *device.get(),
            *tempRenderResources.get(),
            components,
            tempShadowManager.get()};

        // GBuffer only needs the Camera.
        device->bindDataBufferBase(camera->getCameraBufferHandle());
        {
            RenderStateSaver test = RenderStateSaver(device.get());
            gbufferPass->execute(ctx);
        }

        // Change draw target to the main window and clear screen.
        // TODO: Move these to the device.
        device->bindFramebufferBase(0);
        glViewport(0, 0, windowSurfaces[0]->getSize().width, windowSurfaces[0]->getSize().height);
        glClearColor(config.clearColor[0], config.clearColor[1], config.clearColor[2], config.clearColor[3]);
        // By default, this clears color and depth.
        device->clear();

        // We are repeating it in case the binding points were overwritten by
        // any render pass before this.
        //
        // Bind camera and global lights buffer for use in shaders
        device->bindDataBufferBase(camera->getCameraBufferHandle());
        device->bindDataBufferBase(globalLightsBufferHandle);

        {
            RenderStateSaver savedState = RenderStateSaver(device.get());
            deferredRenderPass->execute(ctx);
        }

        // GBuffer only needs the Camera.
        device->bindDataBufferBase(camera->getCameraBufferHandle());
        {
            localLightsPass->execute(ctx);
        }

        device->blitFramebuffers(ctx.resources.framebuffer("gbuffer"), 0, true);
        device->bindFramebufferBase(0);

        // Draw the DebugDraw.
        if ( runDebugDraw ) {
            // Debug Draw Render Pass
            if ( dbrp ) {
                FrameContext dbCtx{
                    0.1f,
                    *device.get(),
                    *tempRenderResources.get(),
                    components};
                {
                    RenderStateSaver test = RenderStateSaver(device.get());
                    dbrp->execute(dbCtx);
                }
            }
        }

        if ( testParticles ) {
            FrameContext prpCtx{
                0.1f,
                *device.get(),
                *tempRenderResources.get(),
                components};

            {
                RenderStateSaver test = RenderStateSaver(device.get());
                prp->execute(prpCtx);
            }
        }

        // UI pass
        // FrameContext for only the ui elements in the scene.
        FrameContext uiCtx{
            0.1f,
            *device.get(),
            *tempRenderResources.get(),
            uiComponents};
        {
            RenderStateSaver test = RenderStateSaver(device.get());
            uiPass->execute(uiCtx);
        }

        windowSurfaces[0]->endFrame();
    }

    void RenderSystem::shutdown() {
        skeletonManager = nullptr;
        meshManager->destroyAllMeshes();
        meshManager = nullptr;
        tempShadowManager = nullptr;
        sp = nullptr;
        uiPass = nullptr;
        prp = nullptr;
        dbrp = nullptr;
        tempRenderResources = nullptr;
        ShaderManager::getInstance().unloadShaders();

        // TODO: Add code to cleanly destroy OpenGL and Surfaces here
    }

    void RenderSystem::registerComponent(const ComponentPtr component) {
        RenderComponentType t = component->GetRenderCompType();


        switch ( component->GetRenderCompType() ) {
        case RenderComponentType::Renderable3D: {
            auto renderComponent = static_cast<Renderable3DComponent*>(component);
            if ( std::shared_ptr<IO::ImageAsset> texAsset = renderComponent->GetImageAsset() ) {
                // Make a texture and assign it to material
                TextureDesc desc = TextureDesc();
                desc.width = static_cast<uint32_t>(texAsset->width);
                desc.height = static_cast<uint32_t>(texAsset->height);
                if ( texAsset->format == IO::ImageAsset::RGB8 )
                    desc.format = Graphics::TextureFormat::RGB8;
                else if ( texAsset->format == IO::ImageAsset::RGBA8 )
                    desc.format = Graphics::TextureFormat::RGBA8;
                else
                    desc.format = Graphics::TextureFormat::RGBA8;
                TextureHandle handle = tempRenderResources->createTexture(texAsset->getName(), desc, "ImageImporter", texAsset->pixelData.data());
                renderComponent->GetMaterial()->setTexture("mainTexture", handle);
                renderComponent->GetMaterial()->setProperty<int>("useTexture", 1);
                // renderComponent->SetImageAsset(nullptr);
            }
            if ( std::shared_ptr<IO::MeshAsset> meshAsset = renderComponent->GetMeshAsset() ) {
                MeshHandle handle = meshManager->getHandle(meshAsset->getName());
                if ( !isValidMesh(handle) ) {
                    if ( auto skinnedMeshAsset = std::dynamic_pointer_cast<IO::SkinnedMeshAsset>(meshAsset) ) {
                        auto skeletonHandle = skeletonManager->createSkeleton(
                            *(skinnedMeshAsset->skeleton.get()));
                        handle = meshManager->createSkinnedMesh(skinnedMeshAsset.get(), skeletonHandle);
                        skinnedMeshAsset->skeleton = nullptr;

                        // Animator setup
                    }
                    else {
                        handle = meshManager->createMesh(meshAsset.get());
                    }
                }
                auto meshResource = meshManager->getMesh(handle);

                renderComponent->SetMesh(handle);
                renderComponent->SetMeshTempAttr(
                    meshResource->gpu.vao,
                    meshResource->gpu.indexCount,
                    meshResource->skinDataBuffer);
                // renderComponent->SetMeshAsset(nullptr);
            }
            else if ( renderComponent->GetMeshPrimitiveType() != PrimitiveMeshType::Invalid ) {
                auto meshHandle = meshManager->createPrimitiveMesh(renderComponent->GetMeshPrimitiveType());
                auto meshResource = meshManager->getMesh(meshHandle);

                renderComponent->SetMesh(meshHandle);
                renderComponent->SetMeshTempAttr(meshResource->gpu.vao, meshResource->gpu.indexCount);
            }
            ISystem::registerComponent(component);
            break;
        }
        case RenderComponentType::Renderable2D:
            // NOTE: This is currently a stub because UI was supposed to have
            // implemented this. Will come back to it later.
            ISystem::registerComponent(component);
            break;
        case RenderComponentType::UIRenderable: {
            auto* uiRenderComp = static_cast<UI::UIRenderComponent*>(component);
            LE_ASSERT("Graphics", uiRenderComp, "Invalid UIRenderComponent.");

            if ( !uiRenderComp->HasMesh() ) {
                auto meshHandle = meshManager->createPrimitiveMesh(
                    PrimitiveMeshType::Square
                );
                auto meshResource = meshManager->getMesh(meshHandle);

                uiRenderComp->SetMesh(meshHandle);
                uiRenderComp->SetMeshTempAttr(
                    meshResource->gpu.vao,
                    meshResource->gpu.indexCount
                );
            }

            if ( std::shared_ptr<IO::ImageAsset> texAsset = uiRenderComp->GetImageAsset() ) {
                // Make a texture and assign it to material
                TextureDesc desc = TextureDesc();
                desc.width = static_cast<uint32_t>(texAsset->width);
                desc.height = static_cast<uint32_t>(texAsset->height);
                if ( texAsset->format == IO::ImageAsset::RGB8 )
                    desc.format = Graphics::TextureFormat::RGB8;
                else if ( texAsset->format == IO::ImageAsset::RGBA8 )
                    desc.format = Graphics::TextureFormat::RGBA8;
                else
                    desc.format = Graphics::TextureFormat::RGBA8;
                TextureHandle handle = tempRenderResources->createTexture(texAsset->getName(), desc, "ImageImporter", texAsset->pixelData.data());
                uiRenderComp->GetMaterial()->setTexture("mainTexture", handle);
                uiRenderComp->GetMaterial()->setProperty<int>("useTexture", 1);
                // renderComponent->SetImageAsset(nullptr);
            }

            uiComponents.push_back(uiRenderComp);
            break;
        }
        case RenderComponentType::UITextRenderable: {
            auto* uiTextComp = static_cast<UI::UITextComponent*>(component);
            LE_ASSERT("Graphics", uiTextComp, "Invalid UITextComponent.");

            // --- Ensure Quad Mesh (same as UI) ---
            if ( !uiTextComp->HasMesh() ) {
                auto meshHandle = meshManager->createPrimitiveMesh(
                    PrimitiveMeshType::Square);
                auto meshResource = meshManager->getMesh(meshHandle);

                uiTextComp->SetMesh(meshHandle);
                uiTextComp->SetMeshTempAttr(
                    meshResource->gpu.vao,
                    meshResource->gpu.indexCount);
            }

            // --- Ensure Font Atlas Exists ---
            auto& FM = FontManager::getInstance();
            auto font = FM.getFont(uiTextComp->GetFontName());
            if ( !font ) {
                // Only load if missing — safe, one-time operation
                std::string filePath = uiTextComp->GetFontName();
                font = FM.loadFont(uiTextComp->GetFontName(), filePath);
            }
            if ( font ) {
                TextureHandle atlas = font->getAtlasTexture();

                LE_ERROR_IF("Graphics", atlas == InvalidTexture, "[UITextRenderable] Font atlas not initialized!");

                // OPTIONAL: if your material system expects a texture slot
                // you can bind it here (even if shader uses u_FontAtlas directly)
                uiTextComp->GetMaterial()->setTexture("u_FontAtlas", atlas);
            }
            else {
                LE_ERROR("Graphics", "[UITextRenderable] Missing font on UITextComponent!");
            }

            // --- Add to UI list ---
            uiComponents.push_back(uiTextComp);
            break;
        }
        case RenderComponentType::PropertyCamera: {
            LE_ASSERT("Graphics", dynamic_cast<CameraComponent*>(component), "Invalid CameraComponent.");
            // TODO: This is currently updating camera to the new component
            // each time. This should be changed when we handle multiple
            // cameras.
            if ( defaultCamera ) {
                defaultCamera = nullptr;
            }
            camera = static_cast<CameraComponent*>(component);
            cameraProperties[camera->GetRenderID()] = camera;
            initCameraBuffer(camera);
            updateCameraBuffer(camera);
            break;
        }
        case RenderComponentType::PropertyLight: {
            LE_ASSERT("Graphics", dynamic_cast<LightComponent*>(component), "Invalid LightComponent.");
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
        case RenderComponentType::Renderable3D: {
            auto renderComponent = static_cast<Renderable3DComponent*>(component);
            // WARNING: Refer to the warning note for meshes, the same applies to
            // textures.
            //
            // auto mat = renderComponent->GetMaterial();

            // WARNING: We cannot delete meshes safely with this system if we
            // want to allow reuse.
            // This should have been taken care of by the Scene.
            // Doing this deletion will break due to duplicate meshes using
            // the same mesh resource.
            //
            // if ( renderComponent->GetMeshPrimitiveType() == PrimitiveMeshType::Invalid ) {
            //     meshManager->destroyMesh(renderComponent->GetMesh());
            // }
            renderComponent->SetMesh(RenderMesh::InvalidMesh);

            ISystem::deregisterComponent(component);
            break;
        }
        case RenderComponentType::Renderable2D:
            ISystem::deregisterComponent(component);
            break;
        case RenderComponentType::UIRenderable:
            //Remove an Element from a vector
            uiComponents.erase(std::remove(uiComponents.begin(), uiComponents.end(), component), uiComponents.end());
            break;
        case RenderComponentType::UITextRenderable:
            // Remove an Element from a vector
            uiComponents.erase(std::remove(uiComponents.begin(), uiComponents.end(), component), uiComponents.end());
            break;
        case RenderComponentType::PropertyCamera: {
            auto it = cameraProperties.find(component->GetRenderID());
            if ( it != cameraProperties.end() ) {
                if ( it->second->getInitStatus() )
                    destroyCameraBuffer(it->second);
                cameraProperties.erase(it);
            }
            break;
        }
        case RenderComponentType::PropertyLight: {
            auto* light = static_cast<LightComponent*>(component);
            deregisterLight(light);
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

    std::shared_ptr<SinglePassShadow> RenderSystem::retrieveSinglePass() {
        return sp;
    }
    std::shared_ptr<ParticleRenderPass> RenderSystem::retrieveParticlePass() {
        return prp;
    }
    std::shared_ptr<SkydomePass> RenderSystem::retrieveSkydomePass() {
        return bp;
    }
    std::shared_ptr<GBufferSkinnedPass> RenderSystem::retrieveGBufferPass() {
        return gbufferPass;
    }
    std::shared_ptr<DebugDrawRenderPass> RenderSystem::retrieveDebugDrawRenderPass() {
        return dbrp;
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
                                                                   DataBuffer::CameraDataBinding},
                                                               nullptr, "CameraData"));
        camera->setInitStatus(true);
    }

    void RenderSystem::updateCameraBuffer(CameraComponent* camera) {
        LE_ASSERT("Graphics", camera, "Camera uninitialized.");
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
            // Temporarily auto adding shadows.
            globalLights.sunLight.shadowIndex = Shadow::ShadowPending;
            if ( isShadowPending(globalLights.sunLight.shadowIndex) ) {
                tempShadowManager->addShadow(0, &globalLights.sunLight, ShadowDesc());
            }
            updateGlobalLights();
            break;
        case LightType::Point:
            // registerLocalLight(light);
            // NOTE: Currently unimplemented, just storing.
            pointLightMapping[light->GetRenderID()] = localLights.pointLights.size();
            localLights.pointLights.push_back(static_cast<PointLightComponent*>(light)->getLightData());
            break;
        case LightType::Default:
        default:
            LE_FATAL_ERROR("Graphics", "Invalid light type.");
            break;
        }

        light->setHandle(globalLightsBufferHandle);
    }

    void RenderSystem::deregisterLight(LightComponent* light) {
        auto it = lightProperties.find(light->GetRenderID());
        if ( it != lightProperties.end() ) {
            switch ( it->second->getType() ) {
            case LightType::Ambient:
                globalLights.ambientLight = AmbientLight{};
                break;
            case LightType::Directional:
                globalLights.sunLight = DirectionalLight{};
                // Temporarily auto adding shadows.
                if ( isShadowPending(globalLights.sunLight.shadowIndex) ) {
                    tempShadowManager->addShadow(0, &globalLights.sunLight, ShadowDesc());
                }
                globalLights.sunLight.shadowIndex = Shadow::NoShadow;
                updateGlobalLights();
                break;
            case LightType::Point:
                localLights.pointLights[pointLightMapping[it->second->GetRenderID()]] = PointLight{};
                break;
            case LightType::Default:
            default:
                LE_FATAL_ERROR("Graphics", "Invalid light type.");
                break;
            }
            lightProperties.erase(it);
            if ( lightProperties.size() == 0 ) {
                tempShadowManager->removeAllShadows();
                localLights.pointLights.clear();
            }
        }
    }

    void RenderSystem::initGlobalLightsBuffer() {
        globalLightsBufferHandle = device->createDataBuffer(
            DataBufferDesc{
                DataBufferType::UBO,
                DataBufferUpdateMode::Dynamic,
                sizeof(GlobalLights),
                DataBuffer::GlobalLightDataBinding},
            &globalLights, "GlobalLightData");
    }

    void RenderSystem::initLocalLightsBuffer() {
        localLightsBufferHandle = device->createDataBuffer(
            DataBufferDesc{
                DataBufferType::SSBO,
                DataBufferUpdateMode::Dynamic,
                sizeof(PointLight) * 200,
                DataBuffer::LocalLightDataBinding},
            "LocalLightData");
    }

    void RenderSystem::updateGlobalLights() const {
        LE_ASSERT("Graphics", isValidDataBuffer(globalLightsBufferHandle), "GlobalLights buffer uninitialized.");
        device->updateDataBufferSubData(
            globalLightsBufferHandle, 0,
            sizeof(GlobalLights), &globalLights);
    }

    void RenderSystem::updateLocalLights() {
        LE_ASSERT("Graphics", isValidDataBuffer(localLightsBufferHandle), "LocalLights buffer uninitialized.");
        LE_ASSERT("Graphics", localLights.pointLights.size() < 200, "LocalLights has exceeded the maximum of 200.");
        for (const auto& light : lightProperties) {
            if ( pointLightMapping.contains(light.second->GetRenderID()) ) {
                auto pointLight = static_cast<PointLightComponent*>(light.second);
                auto& lightData = pointLight->getLightData();
                auto& dst = localLights.pointLights[pointLightMapping[pointLight->GetRenderID()]];

                // Only update dynamic fields
                dst.active = pointLight->isActive();
                dst.position = lightData.position;
                dst.color = lightData.color;
                dst.range = lightData.range;
                dst.intensity = dst.intensity;
            }
        }
        device->updateDataBufferSubData(
            localLightsBufferHandle, 0,
            sizeof(PointLight) * localLights.pointLights.size(), localLights.pointLights.data());
        uint32_t count = static_cast<uint32_t>(localLights.pointLights.size());
        localLightsPass->setLightCount(count);
    }

} // namespace LaurelEye::Graphics
