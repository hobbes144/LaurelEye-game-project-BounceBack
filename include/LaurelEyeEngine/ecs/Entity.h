/// @file   Entity.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @author Anish Murthy (teammate)
///     (anish.murthy.dev@gmail.com) (Code Help and Implementation)
/// @date   9-21-2025
/// @brief  File conatining the core entity structure for the ECS LaurelEyeEngine
/// Lightweight and minimilistic - combines node hierarchy ability with GameObject component
/// architecture
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <stack>
#include <string>
#include "LaurelEyeEngine/math/Transform.h" // Noted dependency, potentially optional
#include "LaurelEyeEngine/math/Vector3.h" // Noted dependency, required
#include "LaurelEyeEngine/ecs/IComponent.h"

namespace LaurelEye {
    static unsigned int nextID;
    class Component;
    class Entity {
    public:
        explicit Entity(std::string name) :
            id(generateId()),
            name(std::move(name)),
            parent(nullptr),
            localTransform(),
            worldTransform(),
            layer(0),
            isLocalSpace(true),
            active(true),
            activeInHierarchy(true),
            markedForDeletion(false) { }

        ~Entity() { shutdown(); }

        // disallow copy
        Entity(const Entity&) = delete;
        Entity& operator=(const Entity&) = delete;

        Entity(Entity&&) = default;
        Entity& operator=(Entity&&) = default;

        /* Hierarchical Methods */
        void addChild(std::unique_ptr<Entity> child);
        void removeChild(Entity* child);
        Entity* findChildByName(const std::string& name);
        bool isActiveInHierarchy() const { return activeInHierarchy; }
        Entity* getParent() const { return parent; }
        std::string getName() const { return name; }
        unsigned int getId() const { return id; }

        /* GameObject Methods */
        void initialize();
        void update(float deltaTime);
        void shutdown();

        void addTag(std::string tagToAdd);
        bool compareTag(std::string tagToCompare);

        /* Component Specific Methods - defined in entity.inl   */
        template <typename ComponentType>
        std::shared_ptr<ComponentType> addComponent();

        template <typename ComponentType>
        std::shared_ptr<ComponentType> addComponent(std::shared_ptr<ComponentType> _component);

        template <typename ComponentType>
        void removeComponent();

        template <typename ComponentType>
        std::shared_ptr<ComponentType> findComponent();

        template <typename ComponentType>
        std::vector<std::shared_ptr<ComponentType>> findComponents();

        template <typename ComponentType>
        std::shared_ptr<ComponentType> findComponentInChild();

        /* Static Methods. Equivalent to Unity Object methods (abstracted from GameObject). May require abstraction*/
        // Instantiate - create an instance of given Entity
        static std::unique_ptr<Entity> instantiate(std::string filepath, std::unique_ptr<Entity> parent = nullptr);
        static std::unique_ptr<Entity> instantiate(Entity original);
        static std::unique_ptr<Entity> instantiate(Entity original, Entity& parent);
        static std::unique_ptr<Entity> instantiate(Entity original, Vector3 position, Quaternion rotation);
        static std::unique_ptr<Entity> instantiate(Entity original, Vector3 position, Quaternion rotation, Entity& parent);

        // Destroy - marks an object for destruction. Scene handles deletion
        static void destroy(Entity* objToDelete);

    private:
        Entity* parent;
        // Only an entity knows about its child entities. Must use the parent entity to access it
        std::vector<std::unique_ptr<Entity>> children;
        std::string name;

        unsigned int id;

        /* Static Next ID counter to ensure every entity gets a unique ID. */
        static unsigned int generateId() {
            static unsigned int counter = 0;
            return counter++;
        }

        std::unordered_set<std::string> tags; // Tags are strings and not constrained. Up to designers to coordinate
        unsigned int layer;                   // identifies the layer the entity is assigned to

        bool isLocalSpace;      // Local space vs world space
        bool active;            // Whether or not to exclude this entity from the main loop
        bool activeInHierarchy; // Whether all parents are active or not
        bool markedForDeletion; // If true, delete this entity at start of next frame

        std::vector<std::shared_ptr<IComponent>> components;

        // TODO - migrate into required component
        Transform localTransform;
        Transform worldTransform;

        /* Helper methods  TODO - note - all transform methods. MOVE THESE OUT */
        void updateTransforms();
        void worldToLocalSpace();
        void localToWorldSpace();

        Transform getLocalTransform() {
            if ( !isLocalSpace )
                worldToLocalSpace();
            return localTransform;
        }

        Transform getWorldTransform() {
            if ( isLocalSpace )
                localToWorldSpace();
            return worldTransform;
        }
    };
    #include "LaurelEyeEngine/ecs/Entity.inl"
} // namespace LaurelEye

