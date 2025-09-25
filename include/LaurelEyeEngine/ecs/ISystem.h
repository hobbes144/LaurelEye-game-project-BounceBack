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

#include <string>
namespace LaurelEye {
    template <typename ComponentType>
    class ISystem {
    public:
        using ComponentPtr = std::shared_ptr<ComponentType>;

        virtual ~ISystem() = default;

        virtual void initialize() = 0;
        virtual void update(float deltaTime) = 0;
        virtual void shutdown() = 0;
        
        void addComponent(const ComponentPtr& component) {
            components.push_back(component);
        }

        void removeComponent(const ComponentPtr& component) {
            components.erase(
                std::remove(components.begin(), components.end(), component),
                components.end());
        }

        void removeComponent(ComponentType* component) {
            components.erase(
                std::remove_if(
                    components.begin(),
                    components.end(),
                    [component](const ComponentPtr& c) { return c.get() == component; }),
                components.end());
        }

    protected:
        std::vector<ComponentPtr> components;
    };
} // namespace LaurelEye
