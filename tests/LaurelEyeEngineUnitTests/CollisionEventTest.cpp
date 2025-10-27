#include "TestDefinitions.h"

namespace LaurelEye {
    void collisionEventTest() {

        std::cout << "------- Collision Event Test -------" << std::endl;

        // === Event Manage ===
        EventManager evtManager;

        // === Temp Enginge Context ===
        EngineContext engineCTX;
        engineCTX.registerService(&evtManager);

        // === Initialize Systems ===
        TransformSystem transformSystem;
        transformSystem.initialize();

        Physics::PhysicsSystem physicsSystem;
        physicsSystem.setEngineContext(engineCTX);
        physicsSystem.initialize();

        // === Create Ground Entity ===
        auto ground = std::make_unique<Entity>("Ground");
        auto groundT = ground->addComponent<TransformComponent>();
        Transform groundLocal;
        groundLocal.setPosition(0.0f, -1.0f, 0.0f);
        groundT->setLocalTransform(groundLocal);
        auto groundPB = ground->addComponent<Physics::PhysicsBodyComponent>(
            Physics::PhysicsBodyData::StaticBox({50.0f, 1.0f, 50.0f}));

        transformSystem.registerComponent(groundT);
        physicsSystem.registerComponent(groundPB);

        // === Create Cube Entity ===
        auto cube = std::make_unique<Entity>("Cube");
        auto cubeT = cube->addComponent<TransformComponent>();
        Transform cubeLocal;
        cubeLocal.setPosition(0.0f, 10.0f, 0.0f);
        cubeT->setLocalTransform(cubeLocal);
        auto cubePB = cube->addComponent<Physics::PhysicsBodyComponent>(
            Physics::PhysicsBodyData::DynamicBox({1.0f, 1.0f, 1.0f}, 1.0f));

        transformSystem.registerComponent(cubeT);
        physicsSystem.registerComponent(cubePB);

        transformSystem.update(0.016);

        // After your update loop setup
        bool enterCalled = false;
        bool stayCalled = false;
        bool exitCalled = false;

        // Add listeners
        uint32_t enterListener = evtManager.addListener<CollisionEnterEvent>(
            [&](const CollisionEnterEvent& event) {
                enterCalled = true;
                auto data = event.GetData();
                assert(data.entityARef && data.entityBRef);
                std::cout << "[Enter] " << data << std::endl;
            });

        uint32_t stayListener = evtManager.addListener<CollisionStayEvent>(
            [&](const CollisionStayEvent& event) {
                stayCalled = true;
                auto data = event.GetData();
                assert(data.entityARef && data.entityBRef);
                std::cout << "[Stay] " << data << std::endl;
            });

        uint32_t exitListener = evtManager.addListener<CollisionExitEvent>(
            [&](const CollisionExitEvent& event) {
                exitCalled = true;
                auto data = event.GetData();
                assert(data.entityARef && data.entityBRef);
                std::cout << "[Exit] " << data << std::endl;
            });

        // Simulation loop
        float dt = 1.0f / 60.0f;
        for ( int i = 0; i < 300; i++ ) {
            physicsSystem.update(dt);
            transformSystem.update(dt);

            // Force exit condition after cube lands
            if ( i == 270 ) {
                auto cubeLocal = cubeT->getLocalTransform();
                cubeLocal.setPosition(0.0f, 2.0f, 0.0f);
                cubeT->setLocalTransform(cubeLocal);
            }
        }

        assert(enterCalled && "Expected CollisionEnterEvent but none received!");
        assert(stayCalled && "Expected CollisionStayEvent but none received!");
        assert(exitCalled && "Expected CollisionExitEvent but none received!");


        physicsSystem.shutdown();
        transformSystem.shutdown();

        std::cout << "-------  End -------" << std::endl;

    }
}
