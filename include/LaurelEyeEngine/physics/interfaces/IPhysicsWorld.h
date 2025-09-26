#pragma once

#include <memory>

#include "LaurelEyeEngine/physics/interfaces/PhysicsTypes.h"

#include "LaurelEyeEngine/physics/interfaces/IBody.h"

namespace LaurelEye::Physics {
    class IBody;
    class ICollisionShape;

    class IPhysicsWorld {
    public:
        virtual ~IPhysicsWorld() = default;
        virtual void StepSimulation(float dt) = 0;
        
        virtual std::shared_ptr<IBody> CreateBody(
            BodyType type,
            const CollisionShapePhys& shapeDesc,
            const TransPhys& start) = 0;
        virtual std::shared_ptr<ICollisionShape> CreateShape(
            const CollisionShapePhys& csPhys) = 0;

        

    protected:
        //TODO: Track Physics Bodies in a more efficient manner
        std::vector<std::shared_ptr<IBody>> bodies;
        std::vector<std::shared_ptr<ICollisionShape>> shapes;
    };
}
