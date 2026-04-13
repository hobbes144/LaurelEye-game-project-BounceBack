///
///
///


#pragma once

#include "IEvent.h"
#include "LaurelEyeEngine/UI/UIComponents/UIInteractionComponent.h"

namespace LaurelEye {

    class UIInteractionEvent : public IEvent {
    public:
        explicit UIInteractionEvent(const LaurelEye::UI::UIInteractionEventData& id)
            : interactionData(id) {
            //LE_DEBUG_INFO("UIInteractionEvent Contrstructor" , id );
        }

        virtual ~UIInteractionEvent() = default;

        LaurelEye::UI::UIInteractionEventData GetData() const { return interactionData; }

    private:
        LaurelEye::UI::UIInteractionEventData interactionData;
    };

    class OnHoverEnterEvent : public UIInteractionEvent {
    public:
        using UIInteractionEvent::UIInteractionEvent; // Inherit constructor
    };

    class OnHoverExitEvent : public UIInteractionEvent {
    public:
        using UIInteractionEvent::UIInteractionEvent; // Inherit constructor
    };

    class OnPressedEvent : public UIInteractionEvent {
    public:
        using UIInteractionEvent::UIInteractionEvent; // Inherit constructor
    };

    class OnHeldEvent : public UIInteractionEvent {
    public:
        using UIInteractionEvent::UIInteractionEvent; // Inherit constructor
    };

    class OnReleasedEvent : public UIInteractionEvent {
    public:
        using UIInteractionEvent::UIInteractionEvent; // Inherit constructor
    };

    class OnClickedEvent : public UIInteractionEvent {
    public:
        using UIInteractionEvent::UIInteractionEvent; // Inherit constructor
    };

    class OnDragStartEvent : public UIInteractionEvent {
    public:
        using UIInteractionEvent::UIInteractionEvent; // Inherit constructor
    };

    class OnDraggingEvent : public UIInteractionEvent {
    public:
        using UIInteractionEvent::UIInteractionEvent; // Inherit constructor
    };

    class OnDragEndEvent : public UIInteractionEvent {
    public:
        using UIInteractionEvent::UIInteractionEvent; // Inherit constructor
    };

    class OnFocusGainedEvent : public UIInteractionEvent {
    public:
        using UIInteractionEvent::UIInteractionEvent; // Inherit constructor
    };

    class OnFocusLostEvent : public UIInteractionEvent {
    public:
        using UIInteractionEvent::UIInteractionEvent; // Inherit constructor
    };

}
