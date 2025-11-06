#include "LaurelEyeEngine/scene/Scene.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/ecs/EntityFactory.h"

// For system initializing
#include "LaurelEyeEngine/transform/TransformComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/Renderable3DComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/CameraComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/LightComponent.h"
#include "LaurelEyeEngine/particles/ParticleEmitterComponent.h"
#include "LaurelEyeEngine/physics/PhysicsBodyComponent.h"
#include "LaurelEyeEngine/scripting/ScriptComponent.h"

#include "LaurelEyeEngine/transform/TransformSystem.h"
#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/scripting/ScriptSystem.h"
#include "LaurelEyeEngine/particles/ParticleSystem.h"
#include "LaurelEyeEngine/io/AssetManager.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"

namespace LaurelEye {

    Scene::Scene(const std::string& name, EngineContext& context, std::shared_ptr<IO::JsonAsset> json, const std::string& assetPath)
        : name(name), active(false), paused(false),
          ctx(context), sceneJson(json), assetRootPath(assetPath) {}
    Scene::~Scene() {
        entities.clear();
        pendingAdditions.clear();
        pendingRemovals.clear();
        sceneJson.reset();
    }

    void Scene::initialize()
    {
        std::cout << "Initializing scene: " << name << std::endl;
        assert(sceneJson && "No valid scene asset for scene.");
        assert(!assetRootPath.empty() && "No valid asset root path for scene.");

        deserialize(sceneJson->jsonDocument);
    }

    void Scene::registerScene() {
        if ( active ) return; // whole scene already registered
        // commit pending additions and register everything
        if ( colorBackground ) {
            ctx.getService<Graphics::RenderSystem>()->setClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z);
            ctx.getService<Graphics::RenderSystem>()->retrieveSkydomePass()->setTexture(Graphics::InvalidTexture);
        }
        else if ( textureBackground ) {
                Graphics::TextureDesc desc = Graphics::TextureDesc();
                desc.width = static_cast<uint32_t>(backgroundTexture->width);
                desc.height = static_cast<uint32_t>(backgroundTexture->height);
                if ( backgroundTexture->format == IO::ImageAsset::RGB8 )
                    desc.format = Graphics::TextureFormat::RGB8;
                else if ( backgroundTexture->format == IO::ImageAsset::RGBA8 )
                    desc.format = Graphics::TextureFormat::RGBA8;
                else
                    desc.format = Graphics::TextureFormat::RGBA8;
                auto handle = ctx.getService<Graphics::RenderSystem>()->getRenderResources()->createTexture(backgroundTexture->getName(), desc, "ImageImporter", backgroundTexture->pixelData.data());
                ctx.getService<Graphics::RenderSystem>()->retrieveSkydomePass()->addTexture(handle);
            }
        auto committed = spawnPendingEntities();
        // Also register any existing entities (for activation we want whole scene)
        for ( auto& e : entities ) {
            if ( !e->getRegistered() ) {
                registerEntityComponents(e.get());
                e->setRegistered(true);
            }
        }
        if ( auto* t = ctx.getService<TransformSystem>() ) t->update(0.016f);
        active = true;
    }

    void Scene::deregisterScene() {
        if ( !active ) return;

        for ( auto& e : entities ) {
            if ( e->getRegistered() ) {
                deregisterEntityComponents(e.get());
                e->setRegistered(false);
            }
        }
            
        active = false;
    }


    void Scene::update(float deltaTime) {
        //std::cout << "Updating scene: " << name << std::endl;
        // Handle any pending add/remove
        spawnPendingEntities();
        cleanupDestroyedEntities();
    }

    void Scene::shutdown() {
        std::cout << "Shutting down scene: " << name << std::endl;

        if ( active ) {
            std::cerr << "Warning: Scene::shutdown called while still active. Force deactivating.\n";
            deregisterScene(); // attempt to make things safe
        }

        entities.clear();
        pendingAdditions.clear();
        pendingRemovals.clear();


        // TODO - for scene saving
        // if (serializeScene){
        // sceneJson->serialize();
        //}
    }

    void Scene::deserialize(const rapidjson::Document& doc) {
        // Check for and set settings
        if ( doc.HasMember("settings") && doc["settings"].IsObject() ) {
            deserializeSettings(doc["settings"], assetRootPath);
        }
        // Deserialize entity list
        EntityFactory factory(ctx, assetRootPath);
        factory.populateSceneFromJson(*this, doc);
    }

    void Scene::deserializeSettings(const rapidjson::Value& settingsValue, const std::string& assetRoot) {
        // Reset on load
        if ( settingsValue.HasMember("resetOnLoad") && settingsValue["resetOnLoad"].IsBool() ) {
            settings.resetOnLoad = settingsValue["resetOnLoad"].GetBool();
        }
        if ( settingsValue.HasMember("backgroundColor") && settingsValue["backgroundColor"].IsArray() ) {
            const auto& color = settingsValue["backgroundColor"];
            backgroundColor = Vector3(color[0].GetFloat(), color[1].GetFloat(), color[2].GetFloat());
            colorBackground = true;
            textureBackground = false;
        }
        else if ( settingsValue.HasMember("backgroundTexture") && settingsValue["backgroundTexture"].IsString() ) {
            std::string backgroundTexturePath = settingsValue["backgroundTexture"].GetString();
            assert(!backgroundTexturePath.empty() && "Background texture path is empty in scene settings.");
            auto texAsset = ctx.getService<IO::AssetManager>()->load(assetRootPath + backgroundTexturePath);
            assert(texAsset && "Failed to load background texture asset in scene settings.");
            backgroundTexture = std::dynamic_pointer_cast<IO::ImageAsset>(texAsset);
            textureBackground = true;
            colorBackground = false;
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

    Entity* Scene::addEntityFromRaw(Entity* entity) {
        if ( !entity ) return nullptr;
        return addEntity(std::unique_ptr<Entity>(entity));
    }

    Entity* Scene::instantiate(const std::string& prefabPath) {
        EntityFactory factory(ctx, assetRootPath);
        Entity* prefabEntity = factory.addPrefabToScene(*this, prefabPath);
        auto newEntities = spawnPendingEntities();
        registerEntities(newEntities);

        if ( auto* t = ctx.getService<TransformSystem>() ) t->update(0.016f);

        return prefabEntity;
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
    std::vector<Entity*> Scene::getEntityPointers() const {
        std::vector<Entity*> ptrs;
        ptrs.reserve(entities.size());
        for ( auto& e : entities ) ptrs.push_back(e.get());
        return ptrs;
    }

    std::vector<Entity*> Scene::spawnPendingEntities() {
        std::vector<Entity*> newPtrs;
        if ( pendingAdditions.empty() ) return newPtrs;

        std::cout << "[Scene] Spawning " << pendingAdditions.size() << " pending entities..." << std::endl;

        size_t count = 0;
        for ( auto& e : pendingAdditions ) {
            if ( !e ) {
                std::cerr << "[Scene] ERROR: Null entity in pending additions!" << std::endl;
                continue;
            }
            std::cout << "  + Adding entity: " << e->getName() << " (ID: " << e->getId() << ")" << std::endl;
            newPtrs.push_back(e.get());
            entities.push_back(std::move(e));
            count++;
        }

        std::cout << "[Scene] Added " << count << " entities. Total now: " << entities.size() << std::endl;

        pendingAdditions.clear();
        return newPtrs;
    }

    void Scene::cleanupDestroyedEntities() {
        if ( pendingRemovals.empty() ) return;
        for ( auto* entity : pendingRemovals ) {
            deregisterEntityComponents(entity);
            entity->setRegistered(false);
        }

        // Then erase all in one pass
        entities.erase(
            std::remove_if(
                entities.begin(),
                entities.end(),
                [&](const std::unique_ptr<Entity>& e) {
                    return std::find(pendingRemovals.begin(), pendingRemovals.end(), e.get()) != pendingRemovals.end();
                }),
            entities.end());

        pendingRemovals.clear();
    }

    void Scene::registerEntityComponents(Entity* entity) {
        if ( !entity ) return;
        auto* transformSystem = ctx.getService<TransformSystem>();
        auto* physicsSystem = ctx.getService<Physics::PhysicsSystem>();
        auto* renderSystem = ctx.getService<Graphics::RenderSystem>();
        auto* scriptingSystem = ctx.getService<Scripting::ScriptSystem>();
        auto* particleSystem = ctx.getService<Particles::ParticleSystem>();

        for (auto& comp : entity->getComponents()) {
            if ( auto* transformComp = dynamic_cast<TransformComponent*>(comp.get()) ) {
                if ( transformSystem ) {
                    transformSystem->registerComponent(transformComp);
                    transformSystem->update(0.016f);
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
            else if ( auto* emitterComp = dynamic_cast<Particles::ParticleEmitterComponent*>(comp.get()) ) {
                if ( particleSystem ) {
                    particleSystem->registerComponent(emitterComp);
                }
            }
            // add more as needed...
        }
    }

    void Scene::deregisterEntityComponents(Entity* entity) {
        if ( !entity ) return;
        auto* transformSystem = ctx.getService<TransformSystem>();
        auto* physicsSystem = ctx.getService<Physics::PhysicsSystem>();
        auto* renderSystem = ctx.getService<Graphics::RenderSystem>();
        auto* scriptingSystem = ctx.getService<Scripting::ScriptSystem>();
        auto* particleSystem = ctx.getService<Particles::ParticleSystem>();

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
            else if ( auto* emitterComp = dynamic_cast<Particles::ParticleEmitterComponent*>(comp.get()) ) {
                if ( particleSystem ) {
                    particleSystem->deregisterComponent(emitterComp);
                }
            }
            // add more as needed...
        }
    }

    void Scene::registerEntities(const std::vector<Entity*>& list) {
        for ( auto* ent : list ) {
            if ( !ent->getRegistered() ) {
                registerEntityComponents(ent);
                ent->setRegistered(true);
            }
        }
    }
} // namespace LaurelEye
