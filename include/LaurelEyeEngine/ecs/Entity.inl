/// @file   Entity.inl
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @author Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
///     (zaldivaralvarez@gmail.com) (Advising and Base Code)
/// @author Alex Humphries (Teammate)
///     (alex.humphries@digipen.edu) (Additional code)
/// @date   9-21-2025
/// @brief  Implementation of templated Entity code. Adapted from GameObject.inl
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once

#include "LaurelEyeEngine/ecs/Entity.h"

/// @brief Add a component by creating
///
/// ## Usage:
///
/// Game behaviour and properties are handled through components. This function
/// adds components to the GameObject by creating them.
///
/// ## Explanation:
///
/// Components that can be added include Render components (requiring Mesh and
/// Material components) and PhysicsBody components.
///
/// ## Note:
///
/// Adding duplicates of a Component type leads to undefined behaviour.
///
/// @return \b std::shared_ptr<ComponentType> Component created
template <typename ComponentType>
ComponentType* LaurelEye::Entity::addComponent() {
    static_assert(std::is_base_of<IComponent, ComponentType>::value,
                  "Component must be derived from IComponent");

    auto component = std::make_unique<ComponentType>();
    component->setOwner(this);

    ComponentType* raw = component.get();
    components.push_back(std::move(component));
    return raw;
}

/// @brief Add a component by creating using Constructor
///  ## Usage:
///
/// Game behaviour and properties are handled through components. This function
/// adds components to the GameObject by creating them. Pass the Componets constructor arguements in
/// as arguments.
///
/// ## Explanation:
///
/// Components that can be added include Render components (requiring Mesh and
/// Material components) and PhysicsBody components.
///
/// ## Note:
///
/// Adding duplicates of a Component type leads to undefined behaviour.
///
/// @param ...args The Arguements to be passed to the constructor
/// @return \b std::shared_ptr<ComponentType> Component created
template <typename ComponentType, typename... Args>
ComponentType* LaurelEye::Entity::addComponent(Args&&... args) {
    // Prevent conflict if someone tries to pass a unique_ptr here
    static_assert(!std::is_same_v<std::decay_t<Args>..., std::unique_ptr<ComponentType>>,
                  "Use addComponent(std::unique_ptr<T>) for unique_ptrs!");

    auto component = std::make_unique<ComponentType>(std::forward<Args>(args)...);
    component->setOwner(this);

    ComponentType* raw = component.get();
    components.push_back(std::move(component));
    return raw;
}

/// @brief Add a component by input, specialized for Renderable
///
/// ## Usage:
///
/// Game behaviour and properties are handled through components. This function
/// adds components to the GameObject by accepting a component as input.
///
/// ## Explanation:
///
/// Components that can be added include Render components (requiring Mesh and
/// Material components) and PhysicsBody components.
///
/// ## Note:
///
/// Adding duplicates of a Component type leads to undefined behaviour.
///
/// @param _component Component to be added to the GameObject
/// @return \b std::shared_ptr<ComponentType> Component added
template <typename ComponentType>
ComponentType* LaurelEye::Entity::addComponent(std::unique_ptr<ComponentType> component) {
    static_assert(std::is_base_of<IComponent, ComponentType>::value,
                  "Component must derive from IComponent");

    component->setOwner(this);

    ComponentType* raw = component.get();
    components.push_back(std::move(component));
    return raw;
}

/// @brief Remove a component
///
/// ## Usage:
///
/// This is to be used when a Entity no longer needs a specific component.
/// Note that this is a heavy call so it is to be avoided as much as possible.
/// Prefer to use events to disable components.
///
/// @param component Component to be removed.
template <typename ComponentType>
void LaurelEye::Entity::removeComponent() {
    auto it = std::remove_if(
        components.begin(),
        components.end(),
        [](const std::unique_ptr<IComponent>& c) {
            return dynamic_cast<ComponentType*>(c.get()) != nullptr;
        });

    if ( it != components.end() ) {
        components.erase(it, components.end());
    }
}

/// @brief Find a component and return a pointer to it
///
/// ## Usage:
///
/// This can be used to get a specific component added to the Entity.
///
/// @return \b <Component derived class>* Component being searched for.
template <typename ComponentType>
ComponentType* LaurelEye::Entity::findComponent() {
    for ( auto& c : components ) {
        if ( auto* casted = dynamic_cast<ComponentType*>(c.get()) ) {
            return casted;
        }
    }
    return nullptr;
}
