#include "LaurelEyeEngine/graphics/graphics_components/Renderable3DComponent.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/graphics/resources/Material.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "TestDefinitions.h"
#include <memory>

namespace LaurelEye {
    void renderTestExtended(IWindow* window, GlfwPlatform* glfwP, InputManager* pInputManager) {
        LaurelEye::Graphics::RenderSystem renderSystem = LaurelEye::Graphics::RenderSystem();
        LaurelEye::Graphics::RenderSystemConfig renderConfig;
        renderConfig.windows.push_back(window);
        renderSystem.setConfig(renderConfig);
        renderSystem.initialize();

        std::cout << "Created window" << std::endl;

        // === Initialize Systems ===
        TransformSystem transformSystem;
        transformSystem.initialize();

        Physics::PhysicsSystem physicsSystem;
        physicsSystem.initialize();

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
        groundRC->GetMaterial()->setProperty<int>("objectId", 3);
        groundRC->GetMaterial()->setProperty<Vector3>("diffuse", Vector3(62.0 / 255.0, 102.0 / 255.0, 38.0 / 255.0));
        groundRC->GetMaterial()->setProperty<Vector3>("specular", Vector3(0.f));
        groundRC->GetMaterial()->setProperty<float>("shininess", 1.f);

        transformSystem.registerComponent(groundT);
        physicsSystem.registerComponent(groundPB);
        renderSystem.registerComponent(groundRC);

        // === Create Cube Entity ===
        auto cube = std::make_unique<Entity>("Cube");
        auto cubeT = cube->addComponent<TransformComponent>();
        Transform cubeLocal;
        cubeLocal.setPosition(0.0f, 10.0f, 0.0f);
        cubeLocal.setScaling(1.0f, 1.0f, 1.0f);
        cubeT->setLocalTransform(cubeLocal);
        auto cubePB = cube->addComponent<Physics::PhysicsBodyComponent>(
            Physics::PhysicsBodyData::DynamicBox({1.0f, 1.0f, 1.0f}, 1.0f));
        auto cubeRC = cube->addComponent<Graphics::Renderable3DComponent>();
        cubeRC->SetMesh(Graphics::Mesh::getShapeMesh(Graphics::Mesh::Cube));
        cubeRC->SetMaterial(std::make_shared<Graphics::Material>());
        cubeRC->GetMaterial()->setProperty<int>("objectId", 9);
        cubeRC->GetMaterial()->setProperty<Vector3>("diffuse", Vector3(0.5f, 0.5f, 0.1f));
        cubeRC->GetMaterial()->setProperty<Vector3>("specular", Vector3(0.009f));
        cubeRC->GetMaterial()->setProperty<float>("shininess", 100.f);

        transformSystem.registerComponent(cubeT);
        physicsSystem.registerComponent(cubePB);
        renderSystem.registerComponent(cubeRC);

        // Simulation loop
        float dt = 1.0f / 60.0f; // 60 Hz
        for ( int i = 0; i < 300.f * 60.0f; i++ ) {
            glfwP->update();
            pInputManager->update();

            if ( pInputManager->isKeyPressed(LaurelEye::Key::Space) || pInputManager->isKeyPressed(LaurelEye::Key::Escape) ) {
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

            physicsSystem.update(dt);
            transformSystem.update(dt);
            renderSystem.update(dt);
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
