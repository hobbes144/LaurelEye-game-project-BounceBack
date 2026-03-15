///
/// @file   UIInteractableSystem.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   01-16-2026
/// @brief  
/// 
///

#pragma once

#include "LaurelEyeEngine/ecs/ISystem.h"
#include "LaurelEyeEngine/UI/UIComponents/UIInteractionComponent.h"
#include "LaurelEyeEngine/math/VectorTemplated.h"

namespace LaurelEye::UI {

    class UIInteractionSystem : public ISystem<UIInteractionComponent> {

    public:
        virtual void initialize() override;
        virtual void update(float deltaTime) override;
        virtual void shutdown() override;

        virtual void registerComponent(const ComponentPtr component) override;
        virtual void deregisterComponent(const ComponentPtr component) override;

        void registerInteractionEventListeners();
        void deregisterInteractionEventListeners();

    private:
        Rect convertRectFromUISpace(Rect r);

        void dispatchInteractionEvents();
        std::vector<UIInteractionEventData> events;

        void generateEvent(
            MouseButton button,
            UIInteractionEventData::UIEventType type,
            UIInteractionComponent* comp,
            const Vector2& screenPos,
            const Vector2& dragDelta = Vector2(0, 0));

        //Interactive Component References
        UIInteractionComponent* hoveredElement = nullptr;
        UIInteractionComponent* pressedElement = nullptr;
        UIInteractionComponent* dragCapturedElement = nullptr;
        UIInteractionComponent* focusedElement = nullptr;

        Vector2 storedWindowSize;

        //Event Listeners
        uint32_t hoverEnterListener;
        uint32_t hoverExitListener;
        uint32_t pressedListener;
        uint32_t heldListener;
        uint32_t releasedListener;
        uint32_t clickedListener;
        uint32_t dragStartListener;
        uint32_t draggingListener;
        uint32_t dragEndListener;
        uint32_t focusGainedListener;
        uint32_t focusLostListener;

    };

}
