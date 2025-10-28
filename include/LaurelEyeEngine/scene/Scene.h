/// @file   Scene.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-21-2025
/// @brief   A level of the game. Holds a flat list of Entity(s). Provides outside access to
/// modifying this list of entities, finding entities, and more.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <rapidjson/document.h>
#include "LaurelEyeEngine/core/EngineContext.h"

namespace LaurelEye {
    class Entity;
    struct SceneSettings {
        bool resetOnLoad = false; // If true the scene resets to default when loaded in. False by default
    };
    class Scene {
    public:
        explicit Scene(const std::string& name);
        ~Scene();

        /* Core scene lifecycle */

        /// @brief Register all entities' components into their respective systems
        void initialize(EngineContext& ctx);
        void update(float deltaTime);
        /// @brief Deregister all entities' components from their respective systems
        void shutdown(EngineContext& ctx);
        /// @brief Given a rapidjson document describing the scene, initialize with the given data
        /// @param doc The rapidjson document describing the state of the scene
        /// @param assetPath localized path to the assets folder for the given game
        void deserialize(const rapidjson::Document& doc, EngineContext& ctx, const std::string& assetRoot);
        /// @brief Deserializes the settings for the scene, setting them into the member variable
        /// @param doc The rapidjson to deserialize
        void deserializeSettings(const rapidjson::Value& settingsValue);

        /* Scene monitoring */
        const std::string& getName() const { return name; }
        bool gsInitialize() const { return initialized; }
        bool isPaused() const { return paused; }

        // --- Entity management (deferred) ---
        Entity* addEntity(std::unique_ptr<Entity> entityToAdd);
        void removeEntity(Entity* entity);
        void removeEntity(const std::string& entityName);
        Entity* findEntityByName(const std::string& name) const;
        Entity* findEntityById(unsigned int id) const;
        std::vector<Entity*> findEntitiesWithTag(const std::string& tag) const; // Note - this is unoptimal for now - use with caution
        const std::vector<std::unique_ptr<Entity>>& getEntities() const { return entities; }

    private:
        void spawnPendingEntities();
        void cleanupDestroyedEntities();

        std::string name;

        std::vector<std::unique_ptr<Entity>> entities; // Flat list of entities for the scene
        std::vector<std::unique_ptr<Entity>> pendingAdditions;
        std::vector<Entity*> pendingRemovals;

        bool initialized;
        bool paused;

        SceneSettings settings;
    };
} // namespace LaurelEye
