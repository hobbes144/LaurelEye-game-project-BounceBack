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

#include <memory>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <string>
#include "LaurelEyeEngine/ecs/IComponent.h"

namespace LaurelEye {
    static unsigned int nextID;
    class Component;
    class Entity {
    public:
        explicit Entity(const std::string& name)
            : id(generateId()),
              name(name),
              active(true) {}

        //Empty constructor, used in memory manager allocation
        Entity()
            : name("unnamed"),
              id(generateId()),
              active(false) {}

        ~Entity() = default;

        // disallow copy
        Entity(const Entity&) = delete;
        Entity& operator=(const Entity&) = delete;

        Entity(Entity&&) = default;
        Entity& operator=(Entity&&) = default;

        /* Component Specific Methods - TODO - when systems own components and their lifespans, remove these  */
        template <typename ComponentType>
        ComponentType* addComponent();

        template <typename ComponentType, typename... Args>
        ComponentType* addComponent(Args&&... args);

        template <typename ComponentType>
        ComponentType* addComponent(std::unique_ptr<ComponentType> component);

        template <typename ComponentType>
        void removeComponent();

        template <typename ComponentType>
        ComponentType* findComponent();

        void setName(std::string newName) { name = newName; }
        const std::string& getName() const { return name; }
        unsigned int getId() const { return id; }

        void setRegistered(bool isRegistered) { registered = isRegistered; }
        bool getRegistered() const { return registered; }

        void setPlayerHealth(int newHealth) { playerHealth = newHealth; }
        int getPlayerHealth() const { return playerHealth; }

        void addTag(std::string tagToAdd) {
            tags.insert(tagToAdd);
        }

        const std::unordered_set<std::string>& getTags() const {
            return tags;
        }

        bool compareTag(std::string tagToCompare) {
            return tags.find(tagToCompare) != tags.end();
        }

        // Clear the entity data, used in memory manager
        void reset() {
            *this = Entity();
        }

        const std::vector<std::unique_ptr<IComponent>>& getComponents() const { return components; }

    private:
        std::string name;
        unsigned int id;
        bool active;
        bool registered = false;

        /* TODO - when systems own components and their lifespans, remove this */
        std::vector<std::unique_ptr<IComponent>> components;

        /* TODO - dedicated UUID generator. Static Next ID counter to ensure every entity gets a unique ID. */
        static unsigned int generateId() {
            static unsigned int counter = 0;
            return counter++;
        }

        // should be moved to data
        int playerHealth = 1;

        std::unordered_set<std::string> tags; // Tags are strings and not constrained. Up to designers to coordinate
        unsigned int layer;                   // identifies the layer the entity is assigned to
    };
} // namespace LaurelEye

#include "LaurelEyeEngine/ecs/Entity.inl"
