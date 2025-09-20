/*!****************************************************************************
 * \file   PhysicsWorld.cpp
 * \author Nicholas Shaw (nick_shaw@me.com)
 * \par    **DigiPen Email**
 *    nick.shaw@digipen.edu
 * \par    **Course**
 *    GAM541
 * \date   09-17-2025
 * \brief  
 * 
 *****************************************************************************/
#include "LaurelEyeEngine/physics/PhysicsWorld.h"

namespace LaurelEye {

    PhysicsWorld::PhysicsWorld() {
        broadphaseInterfaceBLT = new btDbvtBroadphase();
        collisionConfigBLT = new btDefaultCollisionConfiguration();
        dispatcherBLT = new btCollisionDispatcher(collisionConfigBLT);
        solverBLT = new btSequentialImpulseConstraintSolver();
        worldBLT = new btDiscreteDynamicsWorld(dispatcherBLT, broadphaseInterfaceBLT, solverBLT, collisionConfigBLT);
        worldBLT->setGravity(btVector3(0, -9.81f, 0));
    }

    PhysicsWorld::~PhysicsWorld() {
        delete worldBLT;
        delete solverBLT;
        delete dispatcherBLT;
        delete collisionConfigBLT;
        delete broadphaseInterfaceBLT;
    }

    void PhysicsWorld::PhysicsStep(float dt) {
        worldBLT->stepSimulation(dt, 10);
    }

    bool PhysicsWorld::AddBody(Body* body) {
        bodies.push_back(body);
        if ( RigidBody* rb = dynamic_cast<RigidBody*>(body) ) {
            worldBLT->addRigidBody(rb->GetRigidBody());
        }
        else {
            worldBLT->addCollisionObject(body->GetBulletObject());
        }
        return true;
    }
    bool PhysicsWorld::RemoveBody(Body* body) {
        worldBLT->removeCollisionObject(body->GetBulletObject());
        bodies.remove(body);
        return true;
    }

}
