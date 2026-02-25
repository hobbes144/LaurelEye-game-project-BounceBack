#include "TestDefinitions.h"

namespace LaurelEye {
    void inputPhysTest(LaurelEye::GlfwPlatform& glfwP, LaurelEye::InputManager& inputManager) {

        std::cout << "------- Physics + Transform Integration Test -------" << std::endl;

        // === Initialize Systems ===
        LaurelEye::TransformSystem transformSystem;
        transformSystem.initialize();

        Physics::PhysicsSystem physicsSystem;
        physicsSystem.initialize();

        // === Create Ground Entity ===
        auto ground = std::make_unique<LaurelEye::Entity>("Ground");
        auto groundT = ground->addComponent<LaurelEye::TransformComponent>();
        LaurelEye::Transform groundLocal;
        groundLocal.setPosition(0.0f, -1.0f, 0.0f);
        groundLocal.setScaling(10.0f, 1.0f, 10.0f);
        groundT->setLocalTransform(groundLocal);
        groundT->setWorldTransform(groundLocal);
        auto groundPB = ground->addComponent<LaurelEye::Physics::PhysicsBodyComponent>(
            LaurelEye::Physics::PhysicsBodyData::StaticBox({1.0f, 1.0f, 1.0f}));

        transformSystem.registerComponent(groundT);
        physicsSystem.registerComponent(groundPB);

        // === Create Cube Entity ===
        auto cube = std::make_unique<LaurelEye::Entity>("Cube");
        auto cubeT = cube->addComponent<LaurelEye::TransformComponent>();
        LaurelEye::Transform cubeLocal;
        cubeLocal.setPosition(0.0f, 10.0f, 0.0f);
        cubeT->setLocalTransform(cubeLocal);
        cubeT->setWorldTransform(cubeLocal);
        auto cubePB = cube->addComponent<LaurelEye::Physics::PhysicsBodyComponent>(
            LaurelEye::Physics::PhysicsBodyData::DynamicBox({1.0f, 1.0f, 1.0f}, 1.0f));

        transformSystem.registerComponent(cubeT);
        physicsSystem.registerComponent(cubePB);

        //transformSystem.update(0.016);

        // === Simulation ===
        float dt = 1.0f / 60.0f; // 60Hz
        while ( !inputManager.isKeyPressed(LaurelEye::Key::Escape) ) {
            transformSystem.update(dt);
            physicsSystem.update(dt);
            glfwP.update();
            inputManager.update();

            auto cubeWorld = cubeT->getWorldTransform();
            if ( inputManager.isKeyPressed(LaurelEye::Key::Space) ) {
                cubePB->ApplyImpulse(LaurelEye::Vector3(0.0f, 100.0f, 0.0f));
            }
            if ( inputManager.isKeyHeld(LaurelEye::Key::W) ) {
                cubePB->ApplyForce(LaurelEye::Vector3(0.0f, 0.0f, 1.0f));
            }
            // The cube should never sink below ground Y = 0 (since ground is at y=-1 with half height 1)
            // assert(cubeWorld.getPosition().y >= 0.5f - 0.0001f);

            // Debug Printing of Bullet Simulation Body
            auto bulletBody = dynamic_cast<LaurelEye::Physics::BulletRigidBody*>(cubePB->GetBodyRef().get());
            if ( bulletBody ) {
                btTransform btTrans = bulletBody->GetInternal()->getWorldTransform();
                btVector3 pos = btTrans.getOrigin();
                //    std::cout << " Cube Bullet Y: " << pos.getY() << std::endl;
            }
            else {
                std::cout << "Cube body is not a BulletRigidBody!" << std::endl;
            }

            std::cout << " Cube: " << cubeT->getWorldTransform().getPosition() << std::endl;

            // In the early frames, cube should fall (y decreasing)
        }

        // === Final Validation ===
        auto finalPos = cubeT->getWorldTransform().getPosition();

        // Cube should rest roughly on ground surface (y ≈ 0 + halfExtent)
        float expectedRestY = 0.0f; // ground top surface
        assert(std::fabs(finalPos.y - expectedRestY) < 0.05f);

        std::cout << "Cube final Y = " << finalPos.y << " (expected (approx) = " << expectedRestY << ")\n";
        std::cout << "Physics + Transform integration passed!\n";

        physicsSystem.shutdown();
        transformSystem.shutdown();

        std::cout << "------- Physics + Transform Integration End -------" << std::endl;
        
    }
} // namespace LaurelEye
