/// @file   IRenderComponent.h
/// @author Nick Shaw
/// @par    **DigiPen Email**
///     nick.shaw@digipen.edu
/// @date   10-07-2025
/// @brief  Base interface for all renderable and render-property components.
///
/// This file defines the `IRenderComponent` class, which serves as the base
/// class for all components that participate in rendering. It handles
/// transform binding, render ID management, and component classification.
///
/// Derived classes include specific renderable or property components
/// (e.g., Mesh renderers, Lights, and Cameras).
///

#pragma once

#include "LaurelEyeEngine/ecs/IComponent.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"

namespace LaurelEye::Graphics {

    class RenderSystem;

    /// @enum RenderComponentType
    /// @brief Enumerates the different types of render components.
    ///
    /// Used to classify and manage various renderable and render-property
    /// components in the rendering system.
    enum RenderComponentType {
        Renderable3D,
        Renderable2D,
        UIRenderable,
        PropertyCamera,
        PropertyLight
    };

    /// @class IRenderComponent
    /// @brief Base interface for all render-related components.
    ///
    /// This class provides the foundation for any component that affects
    /// rendering. It manages a unique render ID, a bound transform reference,
    /// and the type of render component.
    ///
    /// Common derived classes include `IRenderableComponent` (for mesh/materials)
    /// and camera or light property components.
    class IRenderComponent : public IComponent {

    public:
        /// @brief Constructs an `IRenderComponent` with a specific type.
        /// @param type The render component classification (e.g., `Renderable3D`).
        explicit IRenderComponent(RenderComponentType type) : rcType(type), renderID(++nextID) {}
        /// @brief Virtual destructor.
        virtual ~IRenderComponent();

        /// @brief Gets the unique render ID assigned to this component.
        /// @return The component's render ID.
        unsigned int GetRenderID() const { return renderID; }

        /// @brief Gets the transform currently bound to this render component.
        /// @return Pointer to the bound `TransformComponent`, or `nullptr` if unbound.
        TransformComponent* GetBoundTransform() const {
            if ( rcType == UIRenderable ) {
                std::cerr << "!! Renderable 2D Component Should not have a 3D World bound transform !!" << std::endl;
                std::cerr << "!! Try Swithching to Using UITransformComponent Instead !!" << std::endl;
            }
            return transComp;
        }

        /// @brief Attempts to bind the component's transform from its owning entity.
        ///
        /// This method looks up the owning entity’s `TransformComponent`
        /// and stores a pointer for later use in rendering.
        virtual void BindTransform() {
            if ( rcType == UIRenderable ) {
                std::cerr << "!! UIRenderable Component Should not have a 3D World bound transform !!" << std::endl;
                std::cerr << "!! Try Swithching to Using UITransformComponent Instead !!" << std::endl;
            }
            transComp = owner->findComponent<TransformComponent>();
        }

        /// @brief Gets the type classification of this render component.
        /// @return The `RenderComponentType` value for this component.
        RenderComponentType GetRenderCompType() { return rcType; }

        /// @brief Assigns the owning entity and binds the transform automatically.
        /// @param entity Pointer to the owning `Entity` instance.
        ///
        /// Overrides the base component’s `setOwner()` to ensure the
        /// transform reference is immediately resolved upon assignment.
        virtual void setOwner(Entity* entity) {

            IComponent::setOwner(entity);

            BindTransform();

        }

    protected:
        /// @brief The classification of this render component.
        RenderComponentType rcType;
        /// @brief Cached pointer to the entity’s transform.
        TransformComponent* transComp{nullptr};

    private:
        /// @brief Unique render identifier for this component.
        unsigned int renderID;
        /// @brief Static counter to assign unique render IDs.
        static inline unsigned int nextID = 0;

        RenderSystem* rs = nullptr;

        friend RenderSystem;

    };

}
