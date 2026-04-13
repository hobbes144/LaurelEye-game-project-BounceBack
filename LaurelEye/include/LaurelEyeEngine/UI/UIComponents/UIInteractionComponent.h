/// @file   UIComponent.h
/// @author Daoming Wang
/// @par    **DigiPen Email**
///     daoming.wang@digipen.edu
/// @date   10-30-2025
/// @brief  UI component for the LaurelEye engine.

#pragma once

#include "LaurelEyeEngine/ecs/IComponent.h"
#include "LaurelEyeEngine/input/IInput.h"
#include "LaurelEyeEngine/math/VectorTemplated.h"

namespace LaurelEye::UI {

    struct UIInteractionEventData {

        enum class UIEventType {
            HoverEnter,
            HoverExit,
            Pressed,
            Released,
            Clicked,
            Held,
            DragStart,
            Dragging,
            DragEnd,
            FocusGained,
            FocusLost
        };

        UIEventType type;

        unsigned int entityID;
        Entity* entityRef = nullptr;

        Vector2 screenPosition;
        Vector2 localPosition;

        MouseButton button = MouseButton::Left;
        bool shift = false;
        bool ctrl = false;
        bool alt = false;

        Vector2 dragDelta;

        float timeStamp = 0.0f;

        UIInteractionEventData(
            UIEventType eventType,
            unsigned int entityID,
            Entity* eR)
            : type(eventType), entityID(entityID), entityRef(eR) {}

        UIInteractionEventData(
            UIEventType eventType,
            unsigned int entityID,
            Entity* eR,
            Vector2 screenPos,
            Vector2 localPos,
            MouseButton b,
            bool s,
            bool c,
            bool a) : type(eventType), entityID(entityID), entityRef(eR),
                      screenPosition(screenPos), localPosition(localPos),
                      button(b), shift(s), ctrl(c), alt(a) {}
    };

    /// @class UIComponent
    /// @brief Represents a renderable 2D UI entity in the rendering system.
    ///
    class UIInteractionComponent : public IComponent {
    public:
        //Component Accessors
        bool getIsInteractable() const;
        void setInteractable( bool newI );

        bool getIsBlockingRaycasts() const;
        void setBlockingRaycasts(bool newBR);

        bool getRecievingHover() const;
        void setReceiveHover(bool nv);

        bool getRecievingClick() const;
        void setReceiveClick(bool nv);

        bool getRecievingDrag() const;
        void setReceiveDrag(bool nv);

        bool getDragThreshold() const;
        void setDragThreshold(float nv);

        bool getFocusable() const;
        void setFocusable(bool nv);

        int getInteractionPriority() const;
        void setInteractionPriority(int nv);

        //State Accessors
        bool getIsHovered() const;
        void setIsHovered(bool newIH);

        bool getWasHovered() const;
        void setWasHovered(bool newWH);

        bool getIsPressed() const;
        void setIsPressed(bool newIP);

        bool getWasPressed() const;
        void setWasPressed(bool newWP);

        bool getIsDragging() const;
        void setIsDragging(bool nv);

        Vector2 getPressStartPos() const;
        void setPressStartPos(Vector2 nv);

        Vector2 getDragLastPos() const;
        void setDragLastPos(Vector2 nv);

        Vector2 getDragOffset() const;
        void setDragOffset(Vector2 nv);

    private:
        //Component Variables
        bool isInteractable = true;
        bool blockRaycast = true;

        bool receiveHover = true;
        bool receiveClick = true;
        bool receiveDrag = false;
        float dragThreshold = 10;
        bool focusable = false;

        int interactionPriority = 0;

        //State Variables
        bool isHovered = false;
        bool wasHovered = false;

        bool isPressed = false;
        bool wasPressed = false;

        bool isDragging = false;
        bool wasDragging = false;
        Vector2 pressStartPosition;
        Vector2 dragLastPosition;
        Vector2 dragOffset;

        bool isFocused = false;

        //Telemitry
        float lastClickTime = 0.0f;
        int clickCount = 0;

    };

    inline const char* ToString(UIInteractionEventData::UIEventType type) {
        using Type = UIInteractionEventData::UIEventType;

        switch ( type ) {
        case Type::HoverEnter:
            return "HoverEnter";
        case Type::HoverExit:
            return "HoverExit";
        case Type::Pressed:
            return "Pressed";
        case Type::Released:
            return "Released";
        case Type::Clicked:
            return "Clicked";
        case Type::Held:
            return "Held";
        case Type::DragStart:
            return "DragStart";
        case Type::Dragging:
            return "Dragging";
        case Type::DragEnd:
            return "DragEnd";
        case Type::FocusGained:
            return "FocusGained";
        case Type::FocusLost:
            return "FocusLost";
        default:
            return "Unknown";
        }
    }


    inline std::ostream& operator<<(std::ostream& os, const UIInteractionEventData& data) {
        os << "UIInteractionEventData {\n"
           << "  Type: " << ToString(data.type) << "\n"
           << "  EntityID: " << data.entityID << "\n"
           << "  EntityRef: " << data.entityRef << "\n"
               << "  ScreenPosition: (" << data.screenPosition[0]
               << ", " << data.screenPosition[1] << ")\n"
               << "  LocalPosition: (" << data.localPosition[0]
               << ", " << data.localPosition[1] << ")\n"
           << "  Button: " << static_cast<int>(data.button) << "\n"
           << "  Shift: " << std::boolalpha << data.shift << "\n"
           << "  Ctrl: " << std::boolalpha << data.ctrl << "\n"
           << "  Alt: " << std::boolalpha << data.alt << "\n"
               << "  DragDelta: (" << data.dragDelta[0]
               << ", " << data.dragDelta[1] << ")\n"
           << "  TimeStamp: " << data.timeStamp << "\n"
           << "}";

        return os;
    }

} // namespace LaurelEye::UI


