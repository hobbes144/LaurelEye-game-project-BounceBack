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
#include "LaurelEyeEngine/io/Assets.h"
#include "LaurelEyeEngine/math/Vector3.h"

namespace LaurelEye {
    class Entity;
    struct SceneSettings {
        bool resetOnLoad = false; // If true the scene resets to default when loaded in. False by default
        Vector3 color = Vector3(0.0f, 0.0f, 0.0f); // Background color for the scene
        std::string backgroundTexturePath = ""; // Path to background texture
    };
    class Scene {
    public:
        explicit Scene(const std::string& name, EngineContext& context,
            std::shared_ptr<IO::JsonAsset> sceneJson, const std::string& assetPath);
        ~Scene();

        /* Core scene lifecycle */

        /// @brief Initialize the scene with the given data stub where to load it from,
        /// which are passed in when the scene is created.
        void initialize();
        /// @brief Register the scene. Registers the entities of a scene within their respective system
        void registerScene();
        /// @brief Deregister the scene. Deregisters the entities of a scene within their respective system
        void deregisterScene();
        void update(float deltaTime);
        /// @brief Deregister all entities' components from their respective systems
        void shutdown();
        /// @brief Deserializes the settings for the scene, setting them into the member variable
        /// @param doc The rapidjson to deserialize
        void deserializeSettings(const rapidjson::Value& settingsValue, const std::string& assetRoot);
        void loadUISettings(const rapidjson::GenericArray<true, rapidjson::Value>& uiDatas);

        /* Scene monitoring */
        const std::string& getName() const { return name; }
        bool isPaused() const { return paused; }

        // --- Entity management (deferred) ---
        Entity* addEntity(Entity* entityToAdd);
        /// @brief Allows for creating an entity as a raw pointer, then transferring ownership
        /// over to the scene
        /// @param entity The entity to be transferred
        /// @return The raw entity pointer back out
        Entity* addEntityFromRaw(Entity* entity);

        /// @brief Given a path to a prefab, instantiate an entity at runtime
        /// @param prefabPath The path to the prefab
        /// @param prefabName The scene specific name for the prefab
        Entity* instantiate(const std::string& prefabPath);
        void removeEntity(Entity* entity);
        void removeEntity(const std::string& entityName);
        Entity* findEntityByName(const std::string& name) const;
        Entity* findEntityById(unsigned int id) const;
        std::vector<Entity*> findEntitiesWithTag(const std::string& tag) const; // Note - this is unoptimal for now - use with caution
        const std::vector<Entity*>& getEntities() const { return entities; }
        std::vector<Entity*> getEntityPointers() const;

        bool resetOnLoad() const { return settings.resetOnLoad; }
    private:
        std::vector<Entity*> spawnPendingEntities();
        void cleanupDestroyedEntities();
        void registerEntityComponents(Entity* entity);
        void deregisterEntityComponents(Entity* entity);
        void registerEntities(const std::vector<Entity*>& list);
        /// @brief Given a rapidjson document describing the scene, initialize with the given data
        /// @param doc The rapidjson document describing the state of the scene
        /// @param assetPath localized path to the assets folder for the given game
        void deserialize(const rapidjson::Document& doc);

        EngineContext& ctx;
        std::string name;

        std::vector<Entity*> entities; // Flat list of entities for the scene
        std::vector<Entity*> pendingAdditions;
        std::vector<Entity*> pendingRemovals;

        bool active; // Whether or not the scene is active
        bool paused;

        std::shared_ptr<IO::JsonAsset> sceneJson;
        const std::string& assetRootPath;

        SceneSettings settings;

        Vector3 backgroundColor;
        bool colorBackground = true;
        std::shared_ptr<IO::ImageAsset> backgroundTexture;
        bool textureBackground = false;
    };
} // namespace LaurelEye
