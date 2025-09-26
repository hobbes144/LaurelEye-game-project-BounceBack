#pragma once

#include <btBulletDynamicsCommon.h>

#include "LaurelEyeEngine/physics/interfaces/ICollisionShape.h"

namespace LaurelEye::Physics {

    class BulletShape : public ICollisionShape {
    public:
        BulletShape(const CollisionShapePhys& csPhys, btCollisionShape* shape);
        ~BulletShape();

        CollisionShapePhys GetDiscription() const override;
        btCollisionShape* GetInternal() { return shape; }

    private:
        CollisionShapePhys collisionShapePhys;
        btCollisionShape* shape;

    };

}
