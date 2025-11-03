/// @file   BulletWorld.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10/02/2025
/// @brief Bullet-based implementation of IPhysicsWorld.
/// Manages Bullet world setup, simulation stepping,
/// and creation of bodies and collision shapes.

#pragma once

#include <memory>
#include <btBulletDynamicsCommon.h>

#include "LaurelEyeEngine/physics/interfaces/IPhysicsWorld.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletBody.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletShape.h"

namespace LaurelEye::Physics {
    class BulletWorld : public IPhysicsWorld {
    public:
        BulletWorld();
        ~BulletWorld();

        void StepSimulation(float dt) override;

        std::shared_ptr<IBody> CreateBody(const PhysicsBodyData& data) override;

        void RemoveBody(std::shared_ptr<IBody> body);

        std::shared_ptr<ICollisionShape> CreateShape(
            const CollisionShapePhys& csPhys) override;

        void GatherCollisions(CollisionManager& cm) override;

    private:
        std::vector<std::shared_ptr<btDefaultMotionState>> motions;

        std::unique_ptr<btConstraintSolver> solver;
        std::unique_ptr<btBroadphaseInterface> broadphase;
        std::unique_ptr<btCollisionDispatcher> dispatcher;
        std::unique_ptr<btCollisionConfiguration> config;
        std::unique_ptr<btDiscreteDynamicsWorld> world;

    };
}
