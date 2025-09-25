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
     /**
     * @brief Pending entity that will be added to the scene between frames.
     * If parent is null, the entity is added under the scene root.
     */
    struct PendingEntity {
        std::unique_ptr<Entity> entity;
        Entity* parent;
    };

    class Scene {
    public:
        explicit Scene(const std::string& name);
        virtual ~Scene() = default;

        /* Core scene lifecycle */
        virtual void initialize();
        virtual void update(float deltaTime);
        virtual void shutdown();

        /* Scene monitoring */
        const std::string& getName() const { return name; }
        Entity* getRootEntity() const { return rootEntity.get(); }
        bool gsInitialize() const { return initialized; }
        bool isPaused() const { return paused; }

        // --- Entity management (deferred) ---
        void addEntity(PendingEntity entityToAdd);
        void removeEntity(Entity* entity);
        void removeEntity(const std::string& entityName);

    protected:
        void spawnPendingEntities();
        void cleanupDestroyedEntities();

        virtual void OnEnter() = 0;
        virtual void OnExit() = 0;
        virtual void OnResume() = 0;
        virtual void OnPause() = 0;

        std::string name;
        std::unique_ptr<Entity> rootEntity;

        std::vector<PendingEntity> pendingAdditions;
        std::vector<Entity*> pendingRemovals;

        bool initialized;
        bool paused;
    };
} // namespace LaurelEye
