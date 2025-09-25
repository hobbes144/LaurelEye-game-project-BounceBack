#include "LaurelEyeEngine/scene/Scene.h"
#include "LaurelEyeEngine/ecs/Entity.h"

namespace LaurelEye {

    Scene::Scene(const std::string& name) : name(name), initialized(false), paused(false)
    {
        rootEntity = std::make_unique<Entity>("root");
    }

    void Scene::initialize() {
        std::cout << "Initializing scene: " << name << std::endl;
        if ( rootEntity ) {
            rootEntity->initialize();
        }
    }

    void Scene::update(float deltaTime) {
        std::cout << "Updating scene: " << name << std::endl;
        // Handle any pending add/remove first
        spawnPendingEntities();
        cleanupDestroyedEntities();

        if ( rootEntity && rootEntity->isActiveInHierarchy() ) {
            rootEntity->update(deltaTime);
        }
    }

    void Scene::shutdown() {
        std::cout << "Shutting down scene: " << name << std::endl;
        if ( rootEntity ) {
            rootEntity->shutdown();
            rootEntity.reset();
        }

        pendingAdditions.clear();
        pendingRemovals.clear();
    }

    void Scene::addEntity(PendingEntity entityToAdd) {
        pendingAdditions.push_back(std::move(entityToAdd));
    }

    void Scene::removeEntity(Entity* entityToRemove) {
        if ( entityToRemove ) {
            pendingRemovals.push_back(entityToRemove);
        }
    }

    void Scene::removeEntity(const std::string& entityName) {
        Entity* entityToRemove = rootEntity->findChildByName(entityName);
        if ( entityToRemove ) {
            pendingRemovals.push_back(entityToRemove);
        }
    }

    void Scene::spawnPendingEntities() {
        for ( auto& pending : pendingAdditions ) {
            if ( pending.entity ) {
                if ( pending.parent ) {
                    // Child entity
                    pending.parent->addChild(std::move(pending.entity));
                }
                else if ( rootEntity ) {
                    // Default to scene root
                    rootEntity->addChild(std::move(pending.entity));
                }
                else {
                    // If no root yet, set as root
                    rootEntity = std::move(pending.entity);
                }
            }
        }
        pendingAdditions.clear();
    }

    void Scene::cleanupDestroyedEntities() {
        for ( auto* entity : pendingRemovals ) {
            if ( !entity ) continue;

            if ( entity == rootEntity.get() ) {
                // If root entity itself is removed, shut everything down
                entity->shutdown();
                rootEntity.reset();
            }
            else if ( entity->getParent() ) {
                entity->getParent()->removeChild(entity);
            }
        }
        pendingRemovals.clear();
    }
} // namespace LaurelEye
