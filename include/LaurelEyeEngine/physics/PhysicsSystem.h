#pragma once

#include <memory>

#include "LaurelEyeEngine/physics/interfaces/IPhysicsWorld.h"
#include "LaurelEyeEngine/physics/PhysicsBodyComponent.h"
#include "LaurelEyeEngine/ecs/ISystem.h"

namespace LaurelEye::Physics {

    enum class PhysicsSystemType {
        Bullet
    };

    class PhysicsSystem : ISystem<PhysicsBodyComponent> {
    public:
        PhysicsSystem(PhysicsSystemType type = PhysicsSystemType::Bullet);

        void initialize() override;
        void update(float dt) override;
        void shutdown() override;

        std::shared_ptr<ICollisionShape> CreateShape(const CollisionShapePhys& desc);
        std::shared_ptr<IBody> CreateBody(BodyType type,
                                          const CollisionShapePhys& shapeDesc,
                                          const TransPhys& transform);

        IPhysicsWorld* GetWorld();

    private:
        PhysicsSystemType systemType;
        std::unique_ptr<IPhysicsWorld> world;

    };

}
