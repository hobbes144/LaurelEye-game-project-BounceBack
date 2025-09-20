/*!****************************************************************************
 * \file   Body.cpp
 * \author Nicholas Shaw (nick_shaw@me.com)
 * \par    **DigiPen Email**
 *    nick.shaw@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   09-17-2025
 * \brief  
 * 
 *****************************************************************************/
#include "LaurelEyeEngine/physics/Body.h"

namespace LaurelEye {

    //Body
    Body::Body(Collider* col)
        : collider(col), collisionObjectBLT(nullptr) {}

    Body::~Body() {}

    btCollisionObject* Body::GetBulletObject() {
        return collisionObjectBLT;
    }

    //RigidBody
    RigidBody::RigidBody(Collider* col, float mass, const btVector3& pos)
        : Body(col) {
        //Initalize Transform
        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(pos);

        //Calculate Inertia
        btVector3 inertia(0, 0, 0);
        if ( mass != 0.0f ) col->GetShape()->calculateLocalInertia(mass, inertia);

        //Motion State & Rigidbody
        motionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, col->GetShape(), inertia);
        rigidBodyBLT = new btRigidBody(rbInfo);

        collisionObjectBLT = rigidBodyBLT;
    }

    RigidBody::~RigidBody() {
        delete rigidBodyBLT;
        delete motionState;
    }

    void RigidBody::SyncFromBullet() {
        btTransform t;
        rigidBodyBLT->getMotionState()->getWorldTransform(t);
        //TODO: apply to Game Objects Transform
    }

    void RigidBody::SyncToBullet() {
        //TODO: update Bullet to gameobject
    }

    btRigidBody* RigidBody::GetRigidBody() {
        return rigidBodyBLT;
    }

}
