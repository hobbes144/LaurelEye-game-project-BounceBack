/// @file    EventManager.inl
/// @author  Jacob Wilburn (jacoblinwilburn@gmail.com), Alex Humphries
/// @par     **Digipen Email**
///    jacob.wilburn@-igipen.edu
/// @date    10-8-25
/// @brief   File containing the EventManager logic for the LaurelEyeEngine
///
/// public method function : addListener
///       @param: std::function<void(const EventType&)> callback - A lambda function for a templated EventType. Will add a listener for a specific event
///       @returns: id of the listener
///
/// public method function : removeListener
///       @param: uint32_t entitiyId - A unique id for an event listener. Will find a specific listener and safely delete it
///       @returns: void
///
/// public method function : broadcastEvent
///       @param: const EventType& event - A templated EventType. Will find all listeners to this event and call them using this event
///       @returns: void

#pragma once

#include "LaurelEyeEngine/events/EventManager.h"

#include <algorithm>

    template <typename EventType>
    uint32_t LaurelEye::EventManager::addListener(std::function<void(const EventType&)> callback) {
        uint32_t entityId = UUIDGenerator::generateUUID();
        auto& callbackWrappers = listeners[std::type_index(typeid(EventType))];
        callbackWrappers.push_back({entityId, [callback](const IEvent& event) {
                                        // Perform a safe cast to the correct EventType
                                        const EventType& specificEvent = static_cast<const EventType&>(event);
                                        callback(specificEvent);
                                    }});

        return entityId;
    }

    template <typename EventType>
    void LaurelEye::EventManager::removeListener(uint32_t entityId) {
        auto it = listeners.find(std::type_index(typeid(EventType)));
        if ( it != listeners.end() ) {
            auto& callbackWrappers = it->second;
            callbackWrappers.erase(
                std::remove_if(callbackWrappers.begin(), callbackWrappers.end(),
                               [entityId](const CallbackWrapper& wrapper) {
                                   return wrapper.entityId == entityId;
                               }),
                callbackWrappers.end());

            // If there are no more callbacks, remove the event type
            if ( callbackWrappers.empty() ) {
                listeners.erase(it);
            }
        }
    }

    template <typename EventType>
    void LaurelEye::EventManager::broadcastEvent(const EventType& event) const {
        // Uses type_index (https://en.cppreference.com/w/cpp/types/type_index)
        // of the templated type in order to retrieve from the map
        auto it = listeners.find(std::type_index(typeid(EventType)));
        if ( it != listeners.end() ) {
            for ( const auto& wrapper : it->second ) {
                if ( wrapper.callback ) {
                    wrapper.callback(event);
                }
            }
        }
    }

