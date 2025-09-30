/// @file   Scene.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @author Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
///     (zaldivaralvarez@gmail.com) (Advising and Base Code)
/// @date   9-21-2025
/// @brief   A level. Specifically, holds a SceneGraph that holdes all the objects in the scene.
/// Is responsible for calling update and render on the SceneGraph. Can be paused, can also
/// process unique logic when the scene is entered, exited, resumed, paused, etc
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#include <memory>
#include <string>
#include <vector>

namespace LaurelEye {
    class Entity;
    class Scene {
    public:
        explicit Scene(const std::string& name);
        virtual ~Scene() = default;

        /* Core scene lifecycle */
        void initialize();
        void update(float deltaTime);
        void shutdown();

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
    };
} // namespace LaurelEye
