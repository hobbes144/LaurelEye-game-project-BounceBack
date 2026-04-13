/// @file   Engine.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date   10-17-2025
/// @brief  An injectable dependency layer managed by the engine, populated by the Coordinators,
/// and accessed further down the hierarchy (in things like systems or managers).
/// Everything here is considered a "service", and will consist of instances of systems or managers themselves
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.


#pragma once
#include <typeindex>
#include <unordered_map>

struct TimeData {
    float timeScale = 1.0f;

    float unscaledDt = 0.0f;
    float scaledDt = 0.0f;

    float unscaledTime = 0.0f;
    float scaledTime = 0.0f;
};

namespace LaurelEye {
    class EngineContext {
    public:
        /// @brief Maps a service instance to its typing, allowing for access later
        /// @tparam T The type of service being registered
        /// @param service The instance of the service
        template <typename T>
        void registerService(T* service) {
            services[typeid(T)] = service;
        }

        /// @brief Gets a service stored in the context based on the type provided
        /// @tparam T Type of service requested
        /// @return The instance of the service if one exists
        template <typename T>
        T* getService() const {
            auto it = services.find(typeid(T));
            if ( it != services.end() )
                return static_cast<T*>(it->second);
            return nullptr;
        }

        /// @brief Checks to see if the engine context contains the service type templated
        /// @tparam T The type of service to check for
        /// @return true if the service exists else false
        template <typename T>
        bool hasService() const {
            return services.find(typeid(T)) != services.end();
        }

        const TimeData& time() const { return timeData; }

    private:
        std::unordered_map<std::type_index, void*> services;
        TimeData timeData;

        TimeData& mutableTime() { return timeData; }

        friend class Engine;
    };
} // namespace LaurelEye
