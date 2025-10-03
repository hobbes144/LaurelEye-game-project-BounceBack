#include "TestDefinitions.h"

namespace LaurelEye {
    void physicsTest() {

        std::cout << "------- Physics + Transform Integration Test -------" << std::endl;

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

        // === Simulation ===
        float dt = 1.0f / 60.0f; // 60Hz
        for ( int i = 0; i < 300; i++ ) {
            physicsSystem.update(dt);
            transformSystem.update(dt);

            auto cubeWorld = cubeT->getWorldTransform();

            // The cube should never sink below ground Y = 0 (since ground is at y=-1 with half height 1)
            //assert(cubeWorld.getPosition().y >= 0.5f - 0.0001f);

            /*
            auto bulletBody = dynamic_cast<Physics::BulletBody*>(cubePB->GetBodyRef().get());
            if ( bulletBody ) {
                btTransform btTrans = bulletBody->GetInternal()->getWorldTransform();
                btVector3 pos = btTrans.getOrigin();
                std::cout << "Step " << i
                          << " Cube Bullet Y: " << pos.getY()
                          << std::endl;
            }
            else {
                std::cout << "Cube body is not a BulletBody!" << std::endl;
            }
            */
            std::cout << "Step " << i
                      << " Cube Y: " << cubeT->getWorldTransform().getPosition().y
                      << std::endl;
            

            // In the early frames, cube should fall (y decreasing)
            if ( i == 10 ) {
                float y0 = cubeWorld.getPosition().y;
                physicsSystem.update(dt);
                transformSystem.update(dt);
                float y1 = cubeT->getWorldTransform().getPosition().y;
                assert(y1 < y0); // falling
            }
        }

        // === Final Validation ===
        auto finalPos = cubeT->getWorldTransform().getPosition();

        // Cube should rest roughly on ground surface (y ≈ 0 + halfExtent)
        float expectedRestY = 0.0f; // ground top surface
        assert(std::fabs(finalPos.y - expectedRestY) < 0.05f);

        std::cout << "Cube final Y = " << finalPos.y << " (expected ≈ " << expectedRestY << ")\n";
        std::cout << "Physics + Transform integration passed!\n";

        physicsSystem.shutdown();
        transformSystem.shutdown();

        std::cout << "------- Physics + Transform Integration End -------" << std::endl;

    }
}
