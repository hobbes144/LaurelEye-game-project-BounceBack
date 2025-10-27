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
        explicit CollisionEvent(const LaurelEye::Physics::CollisionEventData& cd)
            : collisionData(cd) {}

        virtual ~CollisionEvent() = default;

        LaurelEye::Physics::CollisionEventData GetData() const { return collisionData; }

    private:
        LaurelEye::Physics::CollisionEventData collisionData;
    };

    class CollisionEnterEvent : public CollisionEvent {
    public:
        using CollisionEvent::CollisionEvent; // Inherit constructor
    };

    class CollisionStayEvent : public CollisionEvent {
    public:
        using CollisionEvent::CollisionEvent;
    };

    class CollisionExitEvent : public CollisionEvent {
    public:
        using CollisionEvent::CollisionEvent;
    };

} // namespace LaurelEye
