/*!****************************************************************************
 * \file   Body.h
 * \author Nicholas Shaw (nick_shaw@me.com)
 * \par    **DigiPen Email**
 *    nick.shaw@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   09-17-2025
 * \brief  
 * 
 *****************************************************************************/
#ifndef BODY_H
#define BODY_H

#pragma once

#include <btBulletDynamicsCommon.h>
#include "LaurelEyeEngine/physics/Collider.h"

namespace LaurelEye {

    class Body { // Abstract Class
    protected:
        Collider* collider;
        btCollisionObject* collisionObjectBLT;

    public:
        Body(Collider* col);
        virtual ~Body();

        virtual void SyncFromBullet() = 0;
        virtual void SyncToBullet() = 0;
        btCollisionObject* GetBulletObject();

    };

    class RigidBody : public Body {
    private:
        btRigidBody* rigidBodyBLT;
        btMotionState* motionState;

    public:
        RigidBody(Collider* col, float mass, const btVector3& pos);
        ~RigidBody();

        void SyncFromBullet() override;
        void SyncToBullet() override;

        btRigidBody* GetRigidBody();
    };

}

#endif
