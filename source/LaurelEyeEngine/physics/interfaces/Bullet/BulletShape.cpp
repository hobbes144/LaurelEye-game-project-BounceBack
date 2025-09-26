#include "LaurelEyeEngine/physics/interfaces/Bullet/BulletShape.h"

namespace LaurelEye::Physics {
    BulletShape::BulletShape(const CollisionShapePhys& csPhys, btCollisionShape* s)
        : collisionShapePhys(csPhys), shape(s) {}

    BulletShape::~BulletShape() {
        delete shape;
    }

    CollisionShapePhys BulletShape::GetDiscription() const {
        return collisionShapePhys;
    }
}
