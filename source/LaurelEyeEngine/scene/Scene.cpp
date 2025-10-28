#include "LaurelEyeEngine/scene/Scene.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/ecs/EntityFactory.h"

// For system initializing
#include "LaurelEyeEngine/transform/TransformComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/Renderable3DComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/CameraComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/LightComponent.h"
#include "LaurelEyeEngine/physics/PhysicsBodyComponent.h"
#include "LaurelEyeEngine/scripting/ScriptComponent.h"

#include "LaurelEyeEngine/transform/TransformSystem.h"
#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/scripting/ScriptSystem.h"

namespace LaurelEye {

    Scene::Scene(const std::string& name) : name(name), initialized(false), paused(false) {}
    Scene::~Scene() {
        entities.clear();
        pendingAdditions.clear();
        pendingRemovals.clear();
        initialized = false;
    }

    void Scene::initialize(EngineContext& ctx) {
        std::cout << "Initializing scene: " << name << std::endl;
        initialized = true;

        // Refresh the entity list
        spawnPendingEntities();
        cleanupDestroyedEntities();

        auto* transformSystem = ctx.getService<TransformSystem>();
        auto* physicsSystem = ctx.getService<Physics::PhysicsSystem>();
        auto* renderSystem = ctx.getService<Graphics::RenderSystem>();
        auto* scriptingSystem = ctx.getService<Scripting::ScriptSystem>();

        for ( auto& entity : entities ) {
            for ( auto& comp : entity->getComponents() ) {
                if ( auto* transformComp = dynamic_cast<TransformComponent*>( comp.get() )) {
                    if ( transformSystem ) {
                        transformSystem->registerComponent(transformComp);
                    }
                }
                else if ( auto* physComp = dynamic_cast<Physics::PhysicsBodyComponent*>(comp.get()) ) {
                    if ( physicsSystem ) {
                        physicsSystem->registerComponent(physComp);
                    }
                }
                else if ( auto* renderComp = dynamic_cast<Graphics::Renderable3DComponent*>(comp.get()) ) {
                    if ( renderSystem ) {
                        renderSystem->registerComponent(renderComp);
                    }
                }
                else if ( auto* cameraComp = dynamic_cast<Graphics::CameraComponent*>(comp.get()) ) {
                    if ( renderSystem ) {
                        renderSystem->registerComponent(cameraComp);
                    }
                }
                else if ( auto* lightComp = dynamic_cast<Graphics::DirectionalLightComponent*>(comp.get()) ) {
                    if ( renderSystem ) {
                        renderSystem->registerComponent(lightComp);
                    }
                }
                else if ( auto* lightComp = dynamic_cast<Graphics::AmbientLightComponent*>(comp.get()) ) {
                    if ( renderSystem ) {
                        renderSystem->registerComponent(lightComp);
                    }
                }
                else if ( auto* lightComp = dynamic_cast<Graphics::PointLightComponent*>(comp.get()) ) {
                    if ( renderSystem ) {
                        renderSystem->registerComponent(lightComp);
                    }
                }
                else if ( auto* scriptComp = dynamic_cast<Scripting::ScriptComponent*>(comp.get()) ) {
                    if ( scriptingSystem ) {
                        scriptingSystem->registerComponent(scriptComp);
                    }
                }
                // add more as needed...
            }
        }

        // Sync world transforms before initial update frame.
        // TODO - better place for this?
        transformSystem->update(0.016);

        initialized = true;
    }

    void Scene::update(float deltaTime) {
        //std::cout << "Updating scene: " << name << std::endl;
        // Handle any pending add/remove
        spawnPendingEntities();
        cleanupDestroyedEntities();
    }

    void Scene::shutdown(EngineContext& ctx) {
        std::cout << "Shutting down scene: " << name << std::endl;
        // TODO - deregister all stored entities from their systems
        spawnPendingEntities();
        cleanupDestroyedEntities();

        auto* transformSystem = ctx.getService<TransformSystem>();
        auto* physicsSystem = ctx.getService<Physics::PhysicsSystem>();
        auto* renderSystem = ctx.getService<Graphics::RenderSystem>();
        auto* scriptingSystem = ctx.getService<Scripting::ScriptSystem>();

        for ( auto& entity : entities ) {
            for ( auto& comp : entity->getComponents() ) {
                if ( auto* transformComp = dynamic_cast<TransformComponent*>(comp.get()) ) {
                    if ( transformSystem ) {
                        transformSystem->deregisterComponent(transformComp);
                    }
                }
                else if ( auto* physComp = dynamic_cast<Physics::PhysicsBodyComponent*>(comp.get()) ) {
                    if ( physicsSystem ) {
                        physicsSystem->deregisterComponent(physComp);
                    }
                }
                else if ( auto* renderComp = dynamic_cast<Graphics::Renderable3DComponent*>(comp.get()) ) {
                    if ( renderSystem ) {
                        renderSystem->deregisterComponent(renderComp);
                    }
                }
                else if ( auto* cameraComp = dynamic_cast<Graphics::CameraComponent*>(comp.get()) ) {
                    if ( renderSystem ) {
                        renderSystem->deregisterComponent(cameraComp);
                    }
                }
                else if ( auto* lightComp = dynamic_cast<Graphics::DirectionalLightComponent*>(comp.get()) ) {
                    if ( renderSystem ) {
                        renderSystem->deregisterComponent(lightComp);
                    }
                }
                else if ( auto* lightComp = dynamic_cast<Graphics::AmbientLightComponent*>(comp.get()) ) {
                    if ( renderSystem ) {
                        renderSystem->deregisterComponent(lightComp);
                    }
                }
                else if ( auto* lightComp = dynamic_cast<Graphics::PointLightComponent*>(comp.get()) ) {
                    if ( renderSystem ) {
                        renderSystem->deregisterComponent(lightComp);
                    }
                }
                else if ( auto* scriptComp = dynamic_cast<Scripting::ScriptComponent*>(comp.get()) ) {
                    if ( scriptingSystem ) {
                        scriptingSystem->deregisterComponent(scriptComp);
                    }
                }
                // add more as needed...
            }
        }
    }

    void Scene::deserialize(const rapidjson::Document& doc, EngineContext& ctx, const std::string& assetRoot) {
        // Check for and set settings
        if ( doc.HasMember("settings") && doc["settings"].IsObject() ) {
            deserializeSettings(doc["settings"]);
        }
        // Deserialize entity list
        EntityFactory factory(ctx, assetRoot);
        factory.populateSceneFromJson(*this, doc);
    }

    void Scene::deserializeSettings(const rapidjson::Value& settingsValue) {
        // Reset on load
        if ( settingsValue.HasMember("resetOnLoad") && settingsValue["resetOnLoad"].IsBool() ) {
            settings.resetOnLoad = settingsValue["resetOnLoad"].GetBool();
        }
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
