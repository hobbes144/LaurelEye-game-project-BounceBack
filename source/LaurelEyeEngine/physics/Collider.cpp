/*!****************************************************************************
 * \file   Collider.cpp
 * \author Nicholas Shaw (nick_shaw@me.com)
 * \par    **DigiPen Email**
 *    nick.shaw@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   09-17-2025
 * \brief  OBSOLETE?
 * 
 *****************************************************************************/
#include "LaurelEyeEngine/physics/Collider.h"

namespace LaurelEye{

    Collider::Collider(ColliderType cType)
        : colliderType(cType), collisionShapeBLT(nullptr) {}

    Collider::~Collider() {
        delete collisionShapeBLT;
    }

    btCollisionShape* Collider::GetShape() const {
        return collisionShapeBLT;
    }

    BoxCollider::BoxCollider(const btVector3& halfExtents)
        : Collider(ColliderType::BoxCollider) {
        collisionShapeBLT = new btBoxShape(halfExtents);
    }

    SphereCollider::SphereCollider(const btScalar& radius)
        : Collider(ColliderType::SphereCollider) {
        collisionShapeBLT = new btSphereShape(radius);
    }

}
