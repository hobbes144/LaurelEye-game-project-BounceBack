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
std::shared_ptr<ComponentType> LaurelEye::Entity::addComponent() {
    static_assert(std::is_base_of<IComponent, ComponentType>::value,
                    "Component must be derived from IComponent class");

    auto component = std::make_shared<ComponentType>();
    component->setOwner(this);
    components.push_back(component);

    return component;
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
std::shared_ptr<ComponentType> LaurelEye::Entity::addComponent(std::shared_ptr<ComponentType> _component) {
    static_assert(std::is_base_of<IComponent, ComponentType>::value,
                    "Component must derive from Component");

    _component->setOwner(this);
    ComponentType* raw = _component.get();
    components.push_back(_component);
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
        [](const std::shared_ptr<IComponent>& c) {
            return dynamic_cast<ComponentType*>(c.get()) != nullptr;
        });

    if ( it != components.end() ) {
        (*it)->shutdown();
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
std::shared_ptr<ComponentType> LaurelEye::Entity::findComponent() {
    for ( auto& c : components ) {
        if ( auto casted = std::dynamic_pointer_cast<ComponentType>(c) ) {
            return casted; // returns a shared_ptr<ComponentType>
        }
    }
    return nullptr;
}

/// @brief Find all components of given type
/// @tparam ComponentType Component type to search for
/// @return The components as a vector of references
template <typename ComponentType>
std::vector<std::shared_ptr<ComponentType>> LaurelEye::Entity::findComponents() {
    std::vector<ComponentType*> results;
    for ( auto& c : components ) {
        if(auto casted = std::dynamic_pointer_cast<ComponentType>(c)) {
            results.push_back(casted);
        }
    }
    return results;
}

/// @brief Find a component from one of the entity's children and return a pointer to it
///
/// ## Usage:
///
/// This can be used to get a specific component added to the Entity's child.
///
/// @return \b <Component derived class>* Component being searched for.
template <class ComponentType>
std::shared_ptr<ComponentType> LaurelEye::Entity::findComponentInChild() {
    std::vector<ComponentType*> results;

    // check this entity
    for ( auto& c : components ) {
        if ( auto* casted = dynamic_cast<ComponentType*>(c.get()) ) {
            results.push_back(casted);
        }
    }

    // recurse into children
    for ( auto& child : children ) {
        auto childResults = child->findComponents<ComponentType>();
        results.insert(results.end(), childResults.begin(), childResults.end());
    }

    return results;
}
