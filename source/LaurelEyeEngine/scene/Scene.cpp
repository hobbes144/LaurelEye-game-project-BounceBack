#include "LaurelEyeEngine/scene/Scene.h"
#include "LaurelEyeEngine/ecs/Entity.h"

namespace LaurelEye {

    Scene::Scene(const std::string& name) : name(name), initialized(false), paused(false) {}

    void Scene::initialize() {
        std::cout << "Initializing scene: " << name << std::endl;
        initialized = true;
    }

    void Scene::update(float deltaTime) {
        std::cout << "Updating scene: " << name << std::endl;
        // Handle any pending add/remove
        spawnPendingEntities();
        cleanupDestroyedEntities();
    }

    void Scene::shutdown() {
        std::cout << "Shutting down scene: " << name << std::endl;
        entities.clear();
        pendingAdditions.clear();
        pendingRemovals.clear();
        initialized = false;
    }

    Entity* Scene::addEntity(std::unique_ptr<Entity> entityToAdd) {
        if ( entityToAdd ) {
            Entity* entityToReturn = entityToAdd.get();
            pendingAdditions.push_back(std::move(entityToAdd));
            return entityToReturn;
        }
        return nullptr;
    }

    void Scene::removeEntity(Entity* entityToRemove) {
        if ( entityToRemove ) {
            pendingRemovals.push_back(entityToRemove);
        }
    }

    void Scene::removeEntity(const std::string& entityName) {
        auto it = std::find_if(
            entities.begin(), entities.end(),
            [&entityName](const std::unique_ptr<Entity>& e) {
                return e->getName() == entityName;
            });
        if ( it != entities.end() ) pendingRemovals.push_back(it->get());
    }

    Entity* Scene::findEntityByName(const std::string& name) const {
        for ( auto& e : entities ) {
            if ( e->getName() == name ) return e.get();
        }
        return nullptr;
    }

    Entity* Scene::findEntityById(unsigned int id) const {
        for ( auto& e : entities ) {
            if ( e->getId() == id ) return e.get();
        }
        return nullptr;
    }

    std::vector<Entity*> Scene::findEntitiesWithTag(const std::string& tag) const {
        std::vector<Entity*> results;
        for ( auto& e : entities ) {
            if ( e->compareTag(tag) ) results.push_back(e.get());
        }
        return results;
    }

    void Scene::spawnPendingEntities() {
        if ( !pendingAdditions.empty() ) {
            entities.insert(
                entities.end(),
                std::make_move_iterator(pendingAdditions.begin()),
                std::make_move_iterator(pendingAdditions.end()));
            pendingAdditions.clear();
        }
    }

    void Scene::cleanupDestroyedEntities() {
        if ( !pendingRemovals.empty() ) {
            for ( Entity* entity : pendingRemovals ) {
                auto it = std::remove_if(
                    entities.begin(), entities.end(),
                    [entity](const std::unique_ptr<Entity>& e) { return e.get() == entity; });
                entities.erase(it, entities.end());
            }
            pendingRemovals.clear();
        }
    }
} // namespace LaurelEye
