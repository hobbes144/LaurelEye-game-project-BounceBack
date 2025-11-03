#include "LaurelEyeEngine/graphics/graphics_components/AmbientLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/DirectionalLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/Renderable3DComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/UIComponent.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/graphics/resources/Material.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "LaurelEyeEngine/io/importers/ImageImporter.h"
#include "TestDefinitions.h"
#include <memory>

namespace LaurelEye {
    void renderTestExtended(IWindow* window, GlfwPlatform* glfwP, InputManager* pInputManager) {
        EngineContext context;
        LaurelEye::Graphics::RenderSystem renderSystem = LaurelEye::Graphics::RenderSystem();
        LaurelEye::Graphics::RenderSystemConfig renderConfig;
        renderConfig.windows.push_back(window);
        renderSystem.setConfig(renderConfig);
        renderSystem.testParticles = false;
        renderSystem.initialize();
        context.registerService<LaurelEye::Graphics::RenderSystem>(&renderSystem);

        std::cout << "Created window" << std::endl;

        // === Initialize Systems ===
        TransformSystem transformSystem;
        transformSystem.initialize();

        Physics::PhysicsSystem physicsSystem;
        physicsSystem.initialize();

        // // === Load player mesh ===
        // auto asset = assetManager.load(std::string(TEST_MEDIA_DIR) + "/item-box.fbx");
        // auto mesh = std::dynamic_pointer_cast<IO::MeshAsset>(asset);

        // === Create Ground Entity ===
        auto ground = std::make_unique<Entity>("Ground");
        auto groundT = ground->addComponent<TransformComponent>();
        Transform groundLocal;
        groundLocal.setPosition(0.0f, -1.0f, 0.0f);
        groundLocal.setScaling(50.0f, 1.0f, 50.0f);
        groundT->setLocalTransform(groundLocal);
        auto groundPB = ground->addComponent<Physics::PhysicsBodyComponent>(
            Physics::PhysicsBodyData::StaticBox({50.0f, 1.0f, 50.0f}));
        auto groundRC = ground->addComponent<Graphics::Renderable3DComponent>();
        groundRC->SetMesh(Graphics::Mesh::getShapeMesh(Graphics::Mesh::Cube));
        groundRC->SetMaterial(std::make_shared<Graphics::Material>());

        // Texture Addition for testing
        LaurelEye::IO::AssetManager assetManager(context);
        assetManager.initialize();

        const std::string importPath = "tests/LaurelEyeEngineUnitTests/TestMedia/textures/test.png";
        auto groundTextureAsset = assetManager.load(importPath);

        // retrieve the GPU handle (importer/register stores texture under the full path)
        auto handle = renderSystem.getRenderResources()->texture(groundTextureAsset->getName());
        // Set shader flags / scale via PropertyMap (uniforms)
        groundRC->GetMaterial()->setProperty<int>("useTexture", 1);
        groundRC->GetMaterial()->setProperty<Vector2>("mainTextureScale", Vector2(1.0f));

        // Bind the texture by name (generic). Name must match the sampler name in the shader.
        // In your fragment shader the sampler is `mainTexture`, so use that name here.
        groundRC->GetMaterial()->setTexture("mainTexture", handle);
        // End Texture Addition

        groundRC->GetMaterial()->setProperty<Vector3>("diffuse", Vector3(62.0 / 255.0, 102.0 / 255.0, 38.0 / 255.0));
        groundRC->GetMaterial()->setProperty<Vector3>("specular", Vector3(0.f));
        groundRC->GetMaterial()->setProperty<float>("shininess", 1.f);

        transformSystem.registerComponent(groundT);
        physicsSystem.registerComponent(groundPB);
        renderSystem.registerComponent(groundRC);

        // === Create Cube Entity ===
        auto cube = std::make_unique<Entity>("Player");
        auto cubeT = cube->addComponent<TransformComponent>();
        Transform cubeLocal;
        cubeLocal.setPosition(0.0f, 10.0f, 0.0f);
        cubeLocal.setScaling(0.05f, 0.05f, 0.05f);
        cubeT->setLocalTransform(cubeLocal);
        auto cubePB = cube->addComponent<Physics::PhysicsBodyComponent>(
            Physics::PhysicsBodyData::DynamicBox({1.0f, 1.0f, 1.0f}, 1.0f));
        auto cubeRC = cube->addComponent<Graphics::Renderable3DComponent>();
        // cubeRC->SetMesh(Graphics::Mesh::loadMesh(std::string(TEST_MEDIA_DIR) + "/models/viking_C.fbx"));
        cubeRC->SetMesh(Graphics::Mesh::loadMesh(std::string(TEST_MEDIA_DIR) + "/models/roman_D.fbx"));
        cubeRC->SetMaterial(std::make_shared<Graphics::Material>());
        cubeRC->GetMaterial()->setProperty<int>("useTexture", 1);

        // retrieve the GPU handle (importer/register stores texture under the full path)
        // const std::string vikingTexPath = std::string(TEST_MEDIA_DIR) + "/textures/viking_blue_C_texture.jpg";
        // auto vikingTex = assetManager.load(vikingTexPath);
        // auto vikingTexHandle = renderSystem.getRenderResources()->texture(vikingTex->getName());
        // cubeRC->GetMaterial()->setTexture("mainTexture", vikingTexHandle);
        const std::string romanTexPath = std::string(TEST_MEDIA_DIR) + "/textures/roman_blue_D_texture.jpg";
        auto romanTex = assetManager.load(romanTexPath);
        auto romanTexHandle = renderSystem.getRenderResources()->texture(romanTex->getName());

        cubeRC->GetMaterial()->setTexture("mainTexture", romanTexHandle);
        // Set shader flags / scale via PropertyMap (uniforms)
        cubeRC->GetMaterial()->setProperty<int>("useTexture", 1);
        cubeRC->GetMaterial()->setProperty<Vector2>("mainTextureScale", Vector2(1.0f));

        cubeRC->GetMaterial()->setProperty<Vector3>("diffuse", Vector3(0.5f, 0.5f, 0.1f));
        cubeRC->GetMaterial()->setProperty<Vector3>("specular", Vector3(0.009f));
        cubeRC->GetMaterial()->setProperty<float>("shininess", 100.f);

        transformSystem.registerComponent(cubeT);
        physicsSystem.registerComponent(cubePB);
        renderSystem.registerComponent(cubeRC);

        auto DLightData = Graphics::DirectionalLight{
            Vector3(0.75f, -1.0f, 0.75f).normalized(), // direction
            4.0f,                                      // intensity
            Vector3(1.0f, 1.0f, 1.0f)                  // color (white)
        };

        auto DLight = std::make_unique<Entity>("DLight");
        auto DLightT = DLight->addComponent<TransformComponent>();
        auto DLightLC = DLight->addComponent<Graphics::DirectionalLightComponent>(DLightData);

        renderSystem.registerComponent(DLightLC);

        auto ALight = std::make_unique<Entity>("ALight");

        auto ALightData = Graphics::AmbientLight{
            Vector3(1.0f, 1.0f, 1.0f), // color (white)
            0.3f                       // intensity
        };

        auto ALightT = ALight->addComponent<TransformComponent>();
        auto ALightLC = ALight->addComponent<Graphics::AmbientLightComponent>(ALightData);

        renderSystem.registerComponent(ALightLC);

        // auto PLightData = Graphics::PointLight{
        //     Vector3(0.0f, 0.0f, 0.0f), // position
        //     50.0f,                     // intensity
        //     Vector3(1.0f, 0.0f, 0.0f), // color (red)
        //     20.0f                      // range
        // };
        //
        // auto PLight = std::make_unique<Entity>("PLight");
        // auto PLightT = PLight->addComponent<TransformComponent>();
        // auto PLightLC = PLight->addComponent<Graphics::PointLightComponent>(PLightData);
        //
        // Transform PLightLocal;
        // PLightLocal.setPosition(0.0f, 15.0f, 0.0f);
        // PLightLocal.setScaling(1.0f, 1.0f, 1.0f);
        // PLightT->setLocalTransform(PLightLocal);
        //
        // renderSystem.registerComponent(PLightLC);

        auto Camera = std::make_unique<Entity>("Camera");
        auto CameraT = Camera->addComponent<TransformComponent>();
        Transform cameraLocal;
        cameraLocal.setPosition(0.f, 15.f, 50.0f);
        cameraLocal.setScaling(1.0f, 1.0f, 1.0f);
        CameraT->setLocalTransform(cameraLocal);

        std::cout << CameraT->getWorldPosition() << std::endl;

        auto CameraComponent = Camera->addComponent<Graphics::CameraComponent>();
        CameraComponent->setPositionRotation(
            Vector3(0.f, 15.f, 50.f),
            Matrix4::lookAt(Vector3(0.f, 15.f, 50.f), Vector3(0.f)).toQuaternion());
        CameraComponent->setPerspectiveProjection(45.0f * 3.14159f / 180.0f, 1280.f / 720.f, 0.1f, 1000.0f);

        renderSystem.registerComponent(CameraComponent);

        transformSystem.update(0.016f);

        // === Create Sprite Entity ===
        auto sprite = std::make_unique<Entity>("Ground");
        auto spriteT = sprite->addComponent<TransformComponent>();
        Transform spriteLocal;
        spriteLocal.setPosition(-0.5f, 0.5f, 0.0f);
        spriteLocal.setScaling(0.25f, 0.25f, 0.0f);
        spriteT->setLocalTransform(spriteLocal);

        auto spriteRC = sprite->addComponent<Graphics::UIComponent>();
        spriteRC->SetMaterial(std::make_shared<Graphics::Material>());

        // Texture Addition for testing
        const std::string importPath1 = "tests/LaurelEyeEngineUnitTests/TestMedia/textures/test.png";
        auto spriteTextureAsset = assetManager.load(importPath1);

        // retrieve the GPU handle (importer/register stores texture under the full path)
        auto handle1 = renderSystem.getRenderResources()->texture(groundTextureAsset->getName());
        // Set shader flags / scale via PropertyMap (uniforms)
        spriteRC->GetMaterial()->setProperty<int>("useTexture", 1);
        spriteRC->GetMaterial()->setProperty<Vector2>("mainTextureScale", Vector2(1.0f));

        // Bind the texture by name (generic). Name must match the sampler name in the shader.
        // In your fragment shader the sampler is `mainTexture`, so use that name here.
        spriteRC->GetMaterial()->setTexture("mainTexture", handle1);
        // End Texture Addition

        spriteRC->GetMaterial()->setProperty<float>("transparency", 1.0f);

        transformSystem.registerComponent(spriteT);
        renderSystem.registerUIComponent(spriteRC);

        // Simulation loop
        float dt = 1.0f / 60.0f; // 60 Hz
        bool paused = false;
        while ( true ) {
            glfwP->update();
            pInputManager->update();

            if ( pInputManager->isKeyPressed(LaurelEye::Key::Space) || pInputManager->isKeyPressed(LaurelEye::Key::Escape) || window->shouldClose() ) {
                groundRC->SetMaterial(nullptr);
                groundRC->SetMesh(nullptr);

                cubeRC->SetMaterial(nullptr);
                cubeRC->SetMesh(nullptr);
                Graphics::Mesh::clearBuffers();
                physicsSystem.shutdown();
                transformSystem.shutdown();
                renderSystem.shutdown();
                return; // bail out of program early
            }
            else if ( pInputManager->isKeyPressed(LaurelEye::Key::Tab) ) {
                paused = !paused;
            }
            else if ( paused && pInputManager->isKeyPressed(LaurelEye::Key::arrowRight) ) {
                physicsSystem.update(dt);
                transformSystem.update(dt);
                renderSystem.update(dt);
            }

            if ( !paused ) {
                physicsSystem.update(dt);
                transformSystem.update(dt);
                renderSystem.update(dt);
            }
#ifdef _WIN32
            Sleep(dt * 1000.f);
#else
            sleep(dt);
#endif
        }

        groundRC->SetMaterial(nullptr);
        groundRC->SetMesh(nullptr);

        cubeRC->SetMaterial(nullptr);
        cubeRC->SetMesh(nullptr);
        Graphics::Mesh::clearBuffers();

        physicsSystem.shutdown();
        transformSystem.shutdown();
        renderSystem.shutdown();
    }
} // namespace LaurelEye
