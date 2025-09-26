
#include "LaurelEyeEngine/physics/interfaces/IBody.h"
#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include <btBulletDynamicsCommon.h>

void PhysicsTest() {
    
    std::cout << "------- Physics Test -------" << std::endl;

    LaurelEye::Physics::PhysicsSystem physicsSystem;
    physicsSystem.initialize();

    // Shape descriptions
    LaurelEye::Physics::CollisionShapePhys groundDesc{
        LaurelEye::Physics::CollisionShapePhys::ShapeType::Box, {50, 1, 50}
    };
    LaurelEye::Physics::CollisionShapePhys cubeDesc{
        LaurelEye::Physics::CollisionShapePhys::ShapeType::Box, {1, 1, 1}
    };

    // Transforms
    LaurelEye::Physics::TransPhys groundTransform{
        {0, -1, 0},
        {1, 0, 0, 0}
    }; // at y = -1

    LaurelEye::Physics::TransPhys cubeTransform{
        {0, 10, 0},
        {1, 0, 0, 0}
    }; // start 10 units up

    // Directly create bodies (no need for CreateShape)
    auto ground = physicsSystem.CreateBody(
        LaurelEye::Physics::BodyType::Static,
        groundDesc,
        groundTransform);

    auto cube = physicsSystem.CreateBody(
        LaurelEye::Physics::BodyType::Dynamic,
        cubeDesc,
        cubeTransform);

    // Simulation loop
    float dt = 1.0f / 60.0f; // 60 Hz
    for ( int i = 0; i < 300; i++ ) {
        physicsSystem.update(dt);

        auto t = cube->GetTransform();
        std::cout << "Step " << i
                  << " | Cube position: ("
                  << t.position.x << ", "
                  << t.position.y << ", "
                  << t.position.z << ")\n";
    }

    physicsSystem.shutdown();

    std::cout << "------- Physics End -------" << std::endl;

}
