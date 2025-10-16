/// @file   IRenderableComponent.h
/// @author Nick Shaw
/// @par    **DigiPen Email**
///         nick.shaw@digipen.edu
/// @date   10-07-2025
/// @brief  Base interface for all renderable components and render properties 
///         within the rendering system, providing shared functionality for 
///         mesh and material management. 

#pragma once

#include "LaurelEyeEngine/graphics/graphics_components/IRenderComponent.h"
#include "LaurelEyeEngine/graphics/resources/Mesh.h"
#include "LaurelEyeEngine/graphics/resources/Material.h"

namespace LaurelEye::Graphics {

    /// @struct RenderableComponentDesc
    /// @brief Description structure containing initialization data for
    ///        renderable components.
    struct RenderableComponentDesc {
        /// @brief Pointer to the mesh resource for this renderable.
        std::shared_ptr<Mesh> mesh;
        /// @brief Pointer to the material resource for this renderable.
        std::shared_ptr<Material> material;

        // Optional: Add other properties later
        // std::string debugName;
        // bool castShadows = true;
    };

    /// @class IRenderableComponent
    /// @brief Base class for all components that can be rendered in the scene.
    ///        Stores references to mesh and material data and provides
    ///        utility functions for managing them.
    class IRenderableComponent : public IRenderComponent {

    public:
        /// @brief Constructs a renderable component of the specified type with
        ///        an optional descriptor.
        /// @param type The render component type (e.g., 2D or 3D renderable).
        /// @param rcd  Descriptor containing mesh and material references.
        explicit IRenderableComponent(RenderComponentType type, RenderableComponentDesc rcd) : IRenderComponent(type) {}
        // @brief Copy constructor. Copies mesh and material references from
        ///        another renderable component.
        /// @param other The renderable component to copy from.
        IRenderableComponent(const IRenderableComponent& other)
            : m_mesh(other.m_mesh), m_material(other.m_material), IRenderComponent(other.rcType) {}

        /// @brief Virtual destructor.
        virtual ~IRenderableComponent() override = default;

        /// @brief Assigns a mesh to this component.
        /// @param mesh Shared pointer to the mesh.
        /// @return Reference to this component (for chaining).
        IRenderableComponent& SetMesh(std::shared_ptr<Mesh> mesh) {
            m_mesh = std::move(mesh);
            return *this;
        }

        /// @brief Assigns a material to this component.
        /// @param material Shared pointer to the material.
        /// @return Reference to this component (for chaining).
        IRenderableComponent& SetMaterial(std::shared_ptr<Material> material) {
            m_material = std::move(material);
            return *this;
        }

        /// @brief Retrieves the mesh assigned to this component.
        /// @return Shared pointer to the mesh.
        std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }
        /// @brief Retrieves the material assigned to this component.
        /// @return Shared pointer to the material.
        std::shared_ptr<Material> GetMaterial() const { return m_material; }

        /// @brief Retrieves the raw mesh pointer.
        /// @return Raw pointer to the mesh, or nullptr if none assigned.
        Mesh* GetMeshRaw() const { return m_mesh.get(); }
        /// @brief Retrieves the raw material pointer.
        /// @return Raw pointer to the material, or nullptr if none assigned.
        Material* GetMaterialRaw() const { return m_material.get(); }

        /// @brief Checks whether this component has an assigned mesh.
        /// @return True if a mesh is assigned, false otherwise.
        bool HasMesh() const { return static_cast<bool>(m_mesh); }
        /// @brief Checks whether this component has an assigned material.
        /// @return True if a material is assigned, false otherwise.
        bool HasMaterial() const { return static_cast<bool>(m_material); }

    protected:
        /// @brief Shared pointer to the mesh resource.
        std::shared_ptr<Mesh> m_mesh;
        /// @brief Shared pointer to the material resource.
        std::shared_ptr<Material> m_material;

    };

} // namespace LaurelEye::Graphics
