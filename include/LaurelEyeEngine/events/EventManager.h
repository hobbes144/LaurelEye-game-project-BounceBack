/// @file    EventManager.h
/// @author  Jacob Wilburn (jacoblinwilburn@gmail.com), Alex Humphries
/// @par     **Digipen Email**
///    jacob.wilburn@digipen.edu
/// @date    10-8-25
/// @brief   File containing the implementation of the EventManager for the LaurelEyeEngine.
/// Can add/remove listeners as well as broadcast events

#pragma once

#include "LaurelEyeEngine/events/IEvent.h"
#include "LaurelEyeEngine/events/UUIDGenerator.h"

#include <cstdint>
#include <functional>
#include <typeindex>

namespace LaurelEye {
    struct CallbackWrapper {
        uint32_t entityId;                           // Object identifier
        std::function<void(const IEvent&)> callback; // Actual callback function
    };

    class EventManager {
    public:
        EventManager() = default;
        ~EventManager() = default;

        template <typename EventType>
        uint32_t addListener(std::function<void(const EventType&)> callback);

        template <typename EventType>
        void removeListener(uint32_t listener);

        template <typename EventType>
        void broadcastEvent(const EventType& event) const;

        template <typename EventType>
        void removeListenersForType() {
            listeners.erase(typeid(EventType));
        }

    private:
        std::unordered_map<std::type_index, std::vector<CallbackWrapper>> listeners;
    };

} // namespace LaurelEye

#include "EventManager.inl"
