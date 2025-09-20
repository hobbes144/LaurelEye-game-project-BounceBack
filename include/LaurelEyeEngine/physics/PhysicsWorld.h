/*!****************************************************************************
 * \file   PhysicsWorld.h
 * \author Nicholas Shaw (nick_shaw@me.com)
 * \par    **DigiPen Email**
 *    nick.shaw@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   09-17-2025
 * \brief  This 
 * 
 *****************************************************************************/
#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#pragma once

#include <list>
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "LaurelEyeEngine/physics/CollisionManager.h"
#include "LaurelEyeEngine/physics/Body.h"

namespace LaurelEye {

    class PhysicsWorld {
    private:
        // Bullet Essentials
        btBroadphaseInterface* broadphaseInterfaceBLT;
        btDefaultCollisionConfiguration* collisionConfigBLT;
        btCollisionDispatcher* dispatcherBLT;
        btSequentialImpulseConstraintSolver* solverBLT;
        btDiscreteDynamicsWorld* worldBLT;

        //Reference to Collision Manager
        CollisionManager collisionManager;

        //Reference to Bodies
        std::list<Body*> bodies; //TODO: Change to match Storage of Game Objects

    public:
        PhysicsWorld();
        ~PhysicsWorld();

        void PhysicsStep(float dt);
        bool AddBody(Body* body);
        bool RemoveBody(Body* body);

        btDiscreteDynamicsWorld* GetWorld() { return worldBLT; }
    };

}

#endif
