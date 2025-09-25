/// @file   IComponent.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @author Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
///     (zaldivaralvarez@gmail.com) (Advising and Base Code)
/// @date   9-21-2025
/// @brief  File containing a Component base class definition. Currently
/// Components can and will execute their own logic (PhysicsComponent executes physics)
/// In the future, this will be mainly data driven.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include <unordered_map>
#include <any>

// TODO - Transition to data driven, removing functions where possible and transitioning to just system data
namespace LaurelEye {
    class Entity;
    class IComponent {
    public:
        IComponent() { active = false; };
        virtual ~IComponent() = default;

        /// @brief Handle any non-memory related initialization here. Things like event listening or variable assigning
        virtual void initialize() = 0;
        
        /// @brief Called by GameObject. Core logic of the component should be handled here. Should modify owner not self.
        /// @param deltaTime - DeltaTime passed in by Engine's Time component
        virtual void update(float deltaTime) = 0;

        /// @brief Memory or non memory related shutdowns can happen here. Ptr destruction for example.
        virtual void shutdown() = 0;

        /// @brief Handles Component configuration using using a map of values. Reads in the map and uses the correlating data to set up this component.
        /// @param properties - The data map mapping a string data name to it's potential value. Need to check for the data name in order to get the correlating value.
        virtual void configure(const std::unordered_map<std::string, std::any>& properties) = 0;

        Entity* getOwner() const { return owner; }
        void setOwner(Entity* entity) { owner = entity; }
        bool isActive() const { return active; }

    protected:
        bool active;
        Entity* owner{nullptr};
    };
}
