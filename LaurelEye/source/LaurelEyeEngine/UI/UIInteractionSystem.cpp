

#include "LaurelEyeEngine/UI/UIInteractionSystem.h"
#include "LaurelEyeEngine/events/UIInteractionEvent.h"
#include "LaurelEyeEngine/input/InputManager.h"
#include "LaurelEyeEngine/events/EventManager.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/graphics/resources/RenderResources.h"
#include "LaurelEyeEngine/UI/UIComponents/UITransformComponent.h"
#include "LaurelEyeEngine/scripting/ScriptComponent.h"
#include "LaurelEyeEngine/math/Rect.h"

#include "LaurelEyeEngine/logging/EngineLog.h"

namespace LaurelEye::UI {

    void UIInteractionSystem::initialize() {

    }

    void UIInteractionSystem::update(float deltaTime) {

        auto renderResources = context->getService<Graphics::RenderSystem>()->getRenderResources();
        const Graphics::SizeRegistry& size = renderResources->getSurfaceSize(0);

        storedWindowSize = Vector2(size.width, size.height);

        auto inputManager = context->getService<InputManager>();

        if ( !inputManager ) return;

        auto mPos = inputManager->getMousePosition();
        Vector2 mousePos(mPos.first, mPos.second);

        bool mousePressed = inputManager->isMouseButtonPressed(MouseButton::Left);
        bool mouseHeld = inputManager->isMouseButtonHeld(MouseButton::Left);
        bool mouseReleased = inputManager->isMouseButtonReleased(MouseButton::Left);

        for ( auto* interactionComp : components ) {

            if ( !interactionComp->isActive() || !interactionComp->getIsInteractable() ) continue;

            auto* entity = interactionComp->getOwner();
            auto* transform = entity->findComponent<UITransformComponent>();
            if ( !transform ) continue;

            const Rect& rect = transform->GetResolvedRect();

            Rect convertedRect = convertRectFromUISpace(rect);

            //TODD: This calculatiuon may be mis represented because of how UI Elements are renderd based vertexes that are too large
            bool containsMouse = convertedRect.Contains(mousePos);

            //Save Previous Component State
            interactionComp->setWasHovered(interactionComp->getIsHovered());
            interactionComp->setWasPressed(interactionComp->getIsPressed());

            //LE_DEBUG_INFO("UIInteraction", "|Mouse Pos:" << mousePos << "|Rect: " << convertedRect << " |ContainsMouse: " << containsMouse);
            //LE_DEBUG_INFO("UIInteraction", " |ContainsMouse: " << containsMouse);

            //Set Is Hovered
            interactionComp->setIsHovered(containsMouse);
            
            if ( !interactionComp->getWasHovered() && interactionComp->getIsHovered() ) {
                //LE_DEBUG_INFO("UIInteraction", "Create OnHoverEnterEvent");
                generateEvent(MouseButton::Left,
                              UIInteractionEventData::UIEventType::HoverEnter,
                              interactionComp,
                              mousePos,
                              Vector2()
                );
            }

            if ( interactionComp->getWasHovered() && !interactionComp->getIsHovered() ) {
                //LE_DEBUG_INFO("UIInteraction", "Create OnHoverExitEvent");
                generateEvent(MouseButton::Left,
                              UIInteractionEventData::UIEventType::HoverExit,
                              interactionComp,
                              mousePos,
                              Vector2());
            }

            if ( containsMouse && mousePressed && !dragCapturedElement) {
                interactionComp->setIsPressed(true);
                interactionComp->setPressStartPos(mousePos);
                interactionComp->setDragLastPos(mousePos);

                dragCapturedElement = interactionComp;

                //LE_DEBUG_INFO("UIInteraction", "Create OnUIPressedEvent");
                generateEvent(MouseButton::Left,
                              UIInteractionEventData::UIEventType::Pressed,
                              interactionComp,
                              mousePos,
                              Vector2());
            }

            Vector2 deltaFromPress;
            if ( interactionComp == dragCapturedElement && interactionComp->getIsPressed() ) {

                deltaFromPress = mousePos - interactionComp->getPressStartPos();
                float distSQ = deltaFromPress.dot(deltaFromPress);

                float thresholdSq = (interactionComp->getDragThreshold()) ^ 2;

                if ( !interactionComp->getIsDragging() && distSQ > thresholdSq ) {
                    interactionComp->setIsDragging(true);

                    //LE_DEBUG_INFO("UIInteraction", "Create OnDragStartEvent");
                    generateEvent(MouseButton::Left,
                                  UIInteractionEventData::UIEventType::DragStart,
                                  interactionComp,
                                  mousePos,
                                  deltaFromPress);
                }

                if (interactionComp->getIsDragging() && mouseHeld ) {

                    Vector2 frameDelta = mousePos - interactionComp->getDragLastPos();

                    //LE_DEBUG_INFO("UIInteraction", "Create OnDraggingEvent");
                    generateEvent(MouseButton::Left,
                                  UIInteractionEventData::UIEventType::Dragging,
                                  interactionComp,
                                  mousePos,
                                  frameDelta);

                    interactionComp->setDragLastPos(mousePos);

                }

            }

            if ( interactionComp == dragCapturedElement &&
                 interactionComp->getIsPressed() &&
                 mouseHeld ) {
                //LE_DEBUG_INFO("UIInteraction", "Create OnUIHeldEvent");
                generateEvent(MouseButton::Left,
                              UIInteractionEventData::UIEventType::Held,
                              interactionComp,
                              mousePos,
                              Vector2());
            }

            if ( interactionComp == dragCapturedElement && interactionComp->getIsPressed() && mouseReleased ) {
                //Create OnUIReleasedEvent
                //LE_DEBUG_INFO("UIInteraction", "Create OnUIReleasedEvent");
                generateEvent(MouseButton::Left,
                              UIInteractionEventData::UIEventType::Released,
                              interactionComp,
                              mousePos,
                              Vector2());

                if ( interactionComp->getIsDragging() ) {
                    //LE_DEBUG_INFO("UIInteraction", "Create OnDragEndEvent");
                    generateEvent(MouseButton::Left,
                                  UIInteractionEventData::UIEventType::DragEnd,
                                  interactionComp,
                                  mousePos,
                                  deltaFromPress);
                }

                if ( containsMouse ) {
                    //Create OnUIClickEvent
                    //LE_DEBUG_INFO("UIInteraction", "Create OnUIClickEvent");
                    generateEvent(MouseButton::Left,
                                  UIInteractionEventData::UIEventType::Clicked,
                                  interactionComp,
                                  mousePos,
                                  Vector2());
                }

                interactionComp->setIsPressed(false);
                interactionComp->setIsDragging(false);
                dragCapturedElement = nullptr;
            }

        }

        dispatchInteractionEvents();

    }

    void UIInteractionSystem::shutdown() {

    }

    void UIInteractionSystem::registerComponent(const ComponentPtr component) {

        LaurelEye::ISystem<UIInteractionComponent>::registerComponent(component);

    }

    void UIInteractionSystem::deregisterComponent(const ComponentPtr component) {

        LaurelEye::ISystem<UIInteractionComponent>::deregisterComponent(component);

    }

    void UIInteractionSystem::registerInteractionEventListeners() {

        auto* eventManager = context->getService<EventManager>();

        LE_DEBUG_ASSERT("UIInteractionSystem", eventManager , "PhysicsSystem: No EventManager in EngineContext");
        // Add the scripting event bindings
        hoverEnterListener = eventManager->addListener<OnHoverEnterEvent>(
            [&](const OnHoverEnterEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityRef);

                if ( auto* scriptA = data.entityRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onHoverEnter(data);
                    }
                }               

            });

        hoverExitListener = eventManager->addListener<OnHoverExitEvent>(
            [&](const OnHoverExitEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityRef);

                if ( auto* scriptA = data.entityRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onHoverExit(data);
                    }
                }
            });

        pressedListener = eventManager->addListener<OnPressedEvent>(
            [&](const OnPressedEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityRef);

                if ( auto* scriptA = data.entityRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onPressed(data);
                    }
                }
            });

        heldListener = eventManager->addListener<OnHeldEvent>(
            [&](const OnHeldEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityRef);

                if ( auto* scriptA = data.entityRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onHeld(data);
                    }
                }
            });

        releasedListener = eventManager->addListener<OnReleasedEvent>(
            [&](const OnReleasedEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityRef);

                if ( auto* scriptA = data.entityRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onReleased(data);
                    }
                }
            });

        clickedListener = eventManager->addListener<OnClickedEvent>(
            [&](const OnClickedEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityRef);

                if ( auto* scriptA = data.entityRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onClicked(data);
                    }
                }
            });

        dragStartListener = eventManager->addListener<OnDragStartEvent>(
            [&](const OnDragStartEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityRef);

                if ( auto* scriptA = data.entityRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onDragStart(data);
                    }
                }
            });

        draggingListener = eventManager->addListener<OnDraggingEvent>(
            [&](const OnDraggingEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityRef);

                if ( auto* scriptA = data.entityRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onDragging(data);
                    }
                }
            });

        dragEndListener = eventManager->addListener<OnDragEndEvent>(
            [&](const OnDragEndEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityRef);

                if ( auto* scriptA = data.entityRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onDragEnd(data);
                    }
                }
            });

        focusGainedListener = eventManager->addListener<OnFocusGainedEvent>(
            [&](const OnFocusGainedEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityRef);

                if ( auto* scriptA = data.entityRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onFocusGained(data);
                    }
                }
            });

        focusLostListener = eventManager->addListener<OnFocusLostEvent>(
            [&](const OnFocusLostEvent& event) {
                const auto& data = event.GetData();
                assert(data.entityRef);

                if ( auto* scriptA = data.entityRef->findComponent<Scripting::ScriptComponent>() ) {
                    if ( auto* instance = scriptA->getScriptInstance() ) {
                        instance->onFocusLost(data);
                    }
                }
            });

    }

    void UIInteractionSystem::deregisterInteractionEventListeners() {

        auto* eventManager = context->getService<EventManager>();
        assert(eventManager && "PhysicsSystem: No EventManager in EngineContext");
        eventManager->removeListener<OnHoverEnterEvent>(hoverEnterListener);
        eventManager->removeListener<OnHoverExitEvent>(hoverExitListener);
        eventManager->removeListener<OnPressedEvent>(pressedListener);
        eventManager->removeListener<OnHeldEvent>(heldListener);
        eventManager->removeListener<OnReleasedEvent>(releasedListener);
        eventManager->removeListener<OnClickedEvent>(clickedListener);
        eventManager->removeListener<OnDragStartEvent>(dragStartListener);
        eventManager->removeListener<OnDraggingEvent>(draggingListener);
        eventManager->removeListener<OnDragEndEvent>(dragEndListener);
        eventManager->removeListener<OnFocusGainedEvent>(focusGainedListener);
        eventManager->removeListener<OnFocusLostEvent>(focusLostListener);

    }

    Rect UIInteractionSystem::convertRectFromUISpace(Rect r) {

        Rect convertedRect = r;

        // Handle Inverted Y Axis
        convertedRect.center[1] = storedWindowSize[1] - convertedRect.center[1];

        // Handles Scaling
        convertedRect.size[0] = convertedRect.size[0] * 2.0f;
        convertedRect.size[1] = convertedRect.size[1] * 2.0f;

        return convertedRect;

    }

    void UIInteractionSystem::dispatchInteractionEvents() {

        if ( !context ) {
            return;
        }
        auto* eventManager = context->getService<EventManager>();
        if ( !eventManager )
            throw std::runtime_error("No Event Manager");

        for ( const auto& evt : events ) {
            // TODO: Use the Event Manager Sigleton or System Coordinator
            //  to Dispatch the events

            switch ( evt.type ) {
            case UIInteractionEventData::UIEventType::HoverEnter: {
                OnHoverEnterEvent hoverEnter(evt);
                eventManager->broadcastEvent(hoverEnter);
                break;
            }
            case UIInteractionEventData::UIEventType::HoverExit: {
                OnHoverExitEvent hoverExit(evt);
                eventManager->broadcastEvent(hoverExit);
                break;
            }
            case UIInteractionEventData::UIEventType::Pressed: {
                OnPressedEvent pressed(evt);
                eventManager->broadcastEvent(pressed);
                break;
            }
            case UIInteractionEventData::UIEventType::Released: {
                OnReleasedEvent released(evt);
                eventManager->broadcastEvent(released);
                break;
            }
            case UIInteractionEventData::UIEventType::Clicked: {
                OnClickedEvent clicked(evt);
                eventManager->broadcastEvent(clicked);
                break;
            }
            case UIInteractionEventData::UIEventType::Held: {
                OnHeldEvent held(evt);
                eventManager->broadcastEvent(held);
                break;
            }
            case UIInteractionEventData::UIEventType::DragStart: {
                OnDragStartEvent dragStart(evt);
                eventManager->broadcastEvent(dragStart);
                break;
            }
            case UIInteractionEventData::UIEventType::Dragging: {
                OnDraggingEvent dragging(evt);
                eventManager->broadcastEvent(dragging);
                break;
            }
            case UIInteractionEventData::UIEventType::DragEnd: {
                OnDragEndEvent dragEnd(evt);
                eventManager->broadcastEvent(dragEnd);
                break;
            }
            case UIInteractionEventData::UIEventType::FocusGained: {
                OnFocusGainedEvent focusGained(evt);
                eventManager->broadcastEvent(focusGained);
                break;
            }
            case UIInteractionEventData::UIEventType::FocusLost: {
                OnFocusLostEvent focusLost(evt);
                eventManager->broadcastEvent(focusLost);
                break;
            }
            default:
                LE_DEBUG_ASSERT("UIInteraction", false, "Unknown collision event type!");
            }

        }

        events.clear();

    }

    /// @brief Shorthand for creating event (HARDCODED TO LEFT MOUSE)
    /// @param type 
    /// @param comp 
    /// @param screenPos 
    /// @param dragDelta 
    void UIInteractionSystem::generateEvent(
        MouseButton button,
        UIInteractionEventData::UIEventType type,
        UIInteractionComponent* comp,
        const Vector2& mousePosition,
        const Vector2& dragDelta)
    {
        auto inputManager = context->getService<InputManager>();

        UIInteractionEventData event(
            type,
            comp->getOwner()->getId(),
            comp->getOwner(),
            mousePosition,
            mousePosition - comp->getOwner()
                            ->findComponent<UITransformComponent>()
                            ->GetResolvedRect()
                            .center,
            button,
            inputManager->isKeyHeld(Key::LShift),
            inputManager->isKeyHeld(Key::LCtrl),
            inputManager->isKeyHeld(Key::LAlt)
        );

        event.dragDelta = dragDelta;
        event.timeStamp = 0.0f; //TODO : Does the engine have a definition of time?

        events.push_back(event);
        
    }

} // namespace LaurelEye::UI
