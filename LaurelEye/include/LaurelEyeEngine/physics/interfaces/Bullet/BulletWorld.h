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
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletRigidBody.h"
#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletGhostBody.h"
//#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletShape.h"

namespace LaurelEye::Physics {
    class BulletWorld : public IPhysicsWorld {
    public:
        BulletWorld();
        ~BulletWorld();

        void StepSimulation(float dt, int maxSubSteps = 1, float fixedTimeStep = 1.0f/60.0f) override;

        std::shared_ptr<IRigidBody> CreateRigidBody(const PhysicsBodyData& data) override;
        //void RemoveBody(std::shared_ptr<IRigidBody> body);

        std::shared_ptr<IGhostBody> CreateGhostBody(const PhysicsBodyData& data) override;
        //void RemoveGhost(std::shared_ptr<IGhostBody> ghost);

        void RemoveObject(std::shared_ptr<ICollider> body) override;

        std::shared_ptr<ICollisionShape> CreateShape(
            const CollisionShapePhys& csPhys) override;

        void GatherCollisions(CollisionManager& cm) override;


        RaycastHit Raycast(const Vector3& origin,
                           const Vector3& direction,
                           float maxDistance,
                           const RaycastParams& params) const override;

    private:
        btCollisionShape* ApplyShapeOffset(
            btCollisionShape* shape,
            const btVector3& pos,
            const btQuaternion& rot);

        //std::vector<std::shared_ptr<BulletGhostBody>> ghosts;
        std::vector<std::shared_ptr<btDefaultMotionState>> motions;

        std::unique_ptr<btConstraintSolver> solver;
        std::unique_ptr<btBroadphaseInterface> broadphase;
        std::unique_ptr<btCollisionDispatcher> dispatcher;
        std::unique_ptr<btCollisionConfiguration> config;
        std::unique_ptr<btDiscreteDynamicsWorld> world;

    };
}
