/*!****************************************************************************
 * \file   Collider.h
 * \author Nicholas Shaw (nick_shaw@me.com)
 * \par    **DigiPen Email**
 *    nick.shaw@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   09-17-2025
 * \brief
 *
 *****************************************************************************/
 #ifndef COLLIDER_H
 #define COLLIDER_H

 #pragma once

 #include "btBulletCollisionCommon.h"

namespace LaurelEye {

    enum ColliderType {
        BoxCollider,
        SphereCollider,
        CapsuleCollider,
        CylinderCollider,
        MeshCollider
    };

    class Collider {
    protected:
        ColliderType colliderType;
        btCollisionShape* collisionShapeBLT;

    public:
        Collider(ColliderType cType);
        virtual ~Collider();

        btCollisionShape* GetShape() const;
    };

    class BoxCollider : public Collider {
    public:
        BoxCollider(const btVector3& halfExtents);
    };

    class SphereCollider : public Collider {
    public:
        SphereCollider(const btScalar& radius);
    };

}

 #endif
