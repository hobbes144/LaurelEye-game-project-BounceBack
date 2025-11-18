/// @file   BulletShape.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10/02/2025
/// @brief Bullet implementation of ICollisionShape.
/// Wraps a btCollisionShape and stores its description.

#pragma once

#include <btBulletDynamicsCommon.h>

#include "LaurelEyeEngine/physics/interfaces/ICollisionShape.h"

namespace LaurelEye::Physics {

    class BulletShape : public ICollisionShape {
    public:
        BulletShape(const CollisionShapePhys& csPhys, btCollisionShape* shape);
        ~BulletShape();

        CollisionShapePhys GetDescription() const override;
        void SetInternal(btCollisionShape* newShape) {
            shape = newShape;
        }
        btCollisionShape* GetInternal() { return shape; }



    private:
        CollisionShapePhys collisionShapePhys;
        btCollisionShape* shape;

    };

}
