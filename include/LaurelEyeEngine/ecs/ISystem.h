/// @file   ISystem.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-22-2025
/// @brief  Interface for systems. A contract directly stating what a System should implement
/// in order to make them generic, and easy to add to the engine.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#pragma once

#include <vector>
#include <algorithm>
#include "LaurelEyeEngine/core/EngineContext.h"

namespace LaurelEye {
    template <typename ComponentType>
    class ISystem {
    public:
        using ComponentPtr = ComponentType*;

        virtual void initialize() = 0;
        virtual void update(float deltaTime) = 0;
        virtual void shutdown() = 0;

        /// @brief Called ONLY ONCE when a system first sees a component.
        /// Sets up any system specific stuff within the component
        /// @param component Component to register
        virtual void registerComponent(const ComponentPtr component) {
            if ( !component ) return;
            components.push_back(component);
        }

        /// @brief Called ONLY ONCE when a system last sees a component.
        /// Tears down any scene specific stuff within the component
        /// @param component Component to deregister
        virtual void deregisterComponent(ComponentPtr component) {
            if ( !component ) return;

            components.erase(
                std::remove(components.begin(), components.end(), component),
                components.end());
        }

        void setEngineContext(EngineContext& newContext) {
            context = &newContext;
        }

    protected:
        EngineContext* context = nullptr;
        std::vector<ComponentPtr> components;
    };
} // namespace LaurelEye
