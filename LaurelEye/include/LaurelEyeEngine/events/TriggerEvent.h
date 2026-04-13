
#pragma once
#include "IEvent.h"
#include "LaurelEyeEngine/physics/CollisionManager.h"

namespace LaurelEye {

    class TriggerEvent : public IEvent {
    public:
        explicit TriggerEvent(const LaurelEye::Physics::CollisionEventData& cd)
            : collisionData(cd) {
            // LE_DEBUG_INFO("Physics", "Trigger Event Contructed");
        }

        virtual ~TriggerEvent() = default;

        LaurelEye::Physics::CollisionEventData GetData() const { return collisionData; }

    private:
        LaurelEye::Physics::CollisionEventData collisionData;
    };

    class TriggerEnterEvent : public TriggerEvent {
    public:
        using TriggerEvent::TriggerEvent; // Inherit constructor
    };

    class TriggerStayEvent : public TriggerEvent {
    public:
        using TriggerEvent::TriggerEvent;
    };

    class TriggerExitEvent : public TriggerEvent {
    public:
        using TriggerEvent::TriggerEvent;
    };

}
