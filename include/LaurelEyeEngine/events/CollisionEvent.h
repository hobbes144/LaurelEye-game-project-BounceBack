/// @file    CollisionEvent.h
/// @author  Jacob Wilburn (jacoblinwilburn@gmail.com)
/// @par     **Digipen Email**
///    jacob.wilburn@digipen.edu
/// @date    10-8-25
/// @brief   File containing the collision event implementation for the LaurelEyeEngine
/// Inherits from the IEvent class

#pragma once
#include "IEvent.h"
#include "LaurelEyeEngine/ecs/Entity.h"

namespace LaurelEye {
    class CollisionEvent : public IEvent {
    public:
        CollisionEvent(Entity* e1, Entity* e2)
            : entity1(e1), entity2(e2) {}

        
        Entity* entity1;
        Entity* entity2;
    };
} // namespace LaurelEye
