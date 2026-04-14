#include "LaurelEyeEngine/scene/Scene.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/ecs/EntityFactory.h"

// For system initializing
#include "LaurelEyeEngine/graphics/graphics_components/AmbientLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/CameraComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/DirectionalLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/PointLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/Renderable3DComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UIRenderComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UITransformComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UIInteractionComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UITextComponent.h"
#include "LaurelEyeEngine/graphics/renderpass/GBufferSkinnedPass.h"
#include "LaurelEyeEngine/graphics/renderpass/SkydomePass.h"
#include "LaurelEyeEngine/particles/ParticleEmitterComponent.h"
#include "LaurelEyeEngine/physics/PhysicsBodyBaseComponent.h"
#include "LaurelEyeEngine/scripting/ScriptComponent.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"

#include "LaurelEyeEngine/audio/AudioSystem.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/graphics/resources/Texture.h"
#include "LaurelEyeEngine/io/AssetManager.h"
#include "LaurelEyeEngine/memory/MemoryManager.h"
#include "LaurelEyeEngine/particles/ParticleSystem.h"
#include "LaurelEyeEngine/physics/PhysicsSystem.h"
#include "LaurelEyeEngine/scripting/ScriptSystem.h"
#include "LaurelEyeEngine/transform/TransformSystem.h"
#include "LaurelEyeEngine/animation/AnimationSystem.h"
#include "LaurelEyeEngine/UI/UILayoutSystem.h"
#include "LaurelEyeEngine/UI/UIInteractionSystem.h"

#include "LaurelEyeEngine/logging/EngineLog.h"

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

    void Scene::initialize() {
        LE_DEBUG_INFO("Scene", "Initializing scene: " << name);
        LE_ASSERT("Scene", sceneJson, "No valid scene asset for scene.");
        LE_ASSERT("Scene", !assetRootPath.empty(), "No valid asset root path for scene.");

        deserialize(sceneJson->jsonDocument);
    }

    void Scene::registerScene() {
        if ( active ) return; // whole scene already registered
        // commit pending additions and register everything
        if ( colorBackground ) {
            ctx.getService<Graphics::RenderSystem>()->setClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z);
            ctx.getService<Graphics::RenderSystem>()->retrieveSkydomePass()->setTexture(Graphics::InvalidTexture);
            ctx.getService<Graphics::RenderSystem>()->retrieveGBufferPass()->addSkydome(Graphics::InvalidTexture);
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
            ctx.getService<Graphics::RenderSystem>()->retrieveGBufferPass()->addSkydome(handle);
        }
        auto committed = spawnPendingEntities();
        // Also register any existing entities (for activation we want whole scene)
        auto entitiesCopy = entities;
        for ( auto& e : entitiesCopy ) {
            if ( !e->getRegistered() ) {
                LE_DEBUG_INFO("Scene", "Address before : " << e << std::endl);
                registerEntityComponents(e);
                LE_DEBUG_INFO("Scene", "Address after: " << e << std::endl);

                // DEBUG CHECK
                if ( !e ) {
                    LE_DEBUG_WARN("Scene", "ENTITY POINTER NULL AFTER REGISTER\n");
                }
                else {
                    LE_DEBUG_INFO("Scene", "Entity after register: " << e->getName() << "\n");
                }

                e->setRegistered(true);
            }
        }
        if ( auto* t = ctx.getService<TransformSystem>() ) t->update(0.016f);
        if ( auto* p = ctx.getService<Physics::PhysicsSystem>() ) p->registerCollisionEnterListeners();
        if ( auto* uiI = ctx.getService<UI::UIInteractionSystem>() ) uiI->registerInteractionEventListeners();

        active = true;
    }

    void Scene::deregisterScene() {
        if ( !active ) return;

        if ( auto* p = ctx.getService<Physics::PhysicsSystem>() ) p->deregisterCollisionEnterListeners();
        if ( auto* uiI = ctx.getService<UI::UIInteractionSystem>() ) uiI->deregisterInteractionEventListeners();

        for ( auto& e : entities ) {
            if ( e->getRegistered() ) {
                deregisterEntityComponents(e);
                e->setRegistered(false);
            }
        }

        active = false;
    }

    void Scene::update(float deltaTime) {
        // std::cout << "Updating scene: " << name << std::endl;
        //  Handle any pending add/remove
        spawnPendingEntities();
        cleanupDestroyedEntities();
    }

    void Scene::shutdown() {
        LE_DEBUG_INFO("Scene", "Shutting down scene: " << name);

        if ( active ) {
            LE_WARN("Scene", "Shutdown called while still active. Force deactivating.");
            deregisterScene(); // attempt to make things safe
        }
        for ( auto& e : entities ) {
            ctx.getService<MemoryManager>()->deallocateMemory(e);
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
            LE_ASSERT("Scene", !backgroundTexturePath.empty(), "Background texture path is empty in scene settings.");
            auto texAsset = ctx.getService<IO::AssetManager>()->load(assetRootPath + backgroundTexturePath);
            LE_ASSERT("Scene", texAsset, "Failed to load background texture asset in scene settings.");
            backgroundTexture = std::dynamic_pointer_cast<IO::ImageAsset>(texAsset);
            textureBackground = true;
            colorBackground = false;
        }
        /*
        if ( settingsValue.HasMember("UIActiveOnLoad") && settingsValue["UIActiveOnLoad"].IsBool() ) {
            if (auto* uiM = ctx.getService<UIElementManager>()) {
                uiM->setShouldUIBeActiveOnStart(settingsValue["UIActiveOnLoad"].GetBool());
            }
        }
        if ( settingsValue.HasMember("UILayout") && settingsValue["UILayout"].IsArray() ) {
            const auto& uiLayout = settingsValue["UILayout"];
            loadUISettings(uiLayout.GetArray());
        }
        */
    }

    Entity* Scene::addEntity(Entity* entityToAdd) {
        if ( entityToAdd ) {
            pendingAdditions.push_back(entityToAdd);

            //Recursivly Add Children to pendingAdditions
            auto transformComponent = entityToAdd->findComponent<TransformComponent>();
            if ( transformComponent ) {
                for ( LaurelEye::TransformComponent* transComponent : transformComponent->getChildren() ) {
                    addEntity(transComponent->getOwner());
                }
            }

            return entityToAdd;
        }
        return nullptr;
    }

    Entity* Scene::addEntityFromRaw(Entity* entity) {
        if ( !entity ) return nullptr;
        return addEntity(entity);
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

            //Recursivly Add Children to pendingRemovals List
            auto transformComponent = entityToRemove->findComponent<TransformComponent>();
            if ( transformComponent ) {
                for ( LaurelEye::TransformComponent* transComponent : transformComponent->getChildren() ) {
                    removeEntity(transComponent->getOwner());
                }
            }

        }
    }

    void Scene::removeEntity(const std::string& entityName) {
        auto it = std::find_if(
            entities.begin(), entities.end(),
            [&entityName](const Entity* e) {
                return e->getName() == entityName;
            });
        if ( it != entities.end() ) {
            pendingRemovals.push_back(*it);

            // Recursivly Add Children to pendingRemovals List
            auto transformComponent = (*it)->findComponent<TransformComponent>();
            if ( transformComponent ) {
                for ( LaurelEye::TransformComponent* transComponent : transformComponent->getChildren() ) {
                    removeEntity(transComponent->getOwner());
                }
            }
        }
    }

    Entity* Scene::findEntityByName(const std::string& name) const {
        for ( auto& e : entities ) {
            if ( e->getName() == name ) {
                return e;
            }
        }
        return nullptr;
    }

    Entity* Scene::findEntityById(unsigned int id) const {
        for ( auto& e : entities ) {
            if ( e->getId() == id ) return e;
        }
        return nullptr;
    }

    std::vector<Entity*> Scene::findEntitiesWithTag(const std::string& tag) const {
        std::vector<Entity*> results;
        for ( auto& e : entities ) {
            if ( e->compareTag(tag) ) results.push_back(e);
        }
        return results;
    }
    std::vector<Entity*> Scene::getEntityPointers() const {
        std::vector<Entity*> ptrs;
        ptrs.reserve(entities.size());
        for ( auto& e : entities ) ptrs.push_back(e);
        return ptrs;
    }

    std::vector<Entity*> Scene::spawnPendingEntities() {
        std::vector<Entity*> newPtrs;
        if ( pendingAdditions.empty() ) return newPtrs;

        LE_DEBUG_INFO("Scene", "Spawning " << pendingAdditions.size() << " pending entities...");

        size_t count = 0;
        for ( auto& e : pendingAdditions ) {
            if ( !e ) {
                LE_DEBUG_ERROR("Scene", "Null entity in pending additions!");
                continue;
            }
            LE_DEBUG_INFO("Scene", "Adding entity: " << e->getName() << " (ID: " << e->getId() << ")");
            newPtrs.push_back(e);
            entities.push_back(std::move(e));
            count++;
        }

        LE_DEBUG_INFO("Scene", "Added " << count << " entities. Total now: " << entities.size());

        pendingAdditions.clear();
        return newPtrs;
    }

    void Scene::cleanupDestroyedEntities() {
        if ( pendingRemovals.empty() ) return;
        for ( auto* entity : pendingRemovals ) {
            if ( entity->getRegistered() ) {
                deregisterEntityComponents(entity);
                entity->setRegistered(false);
            }
        }

        // Then erase all in one pass
        entities.erase(
            std::remove_if(
                entities.begin(),
                entities.end(),
                [&](const Entity* e) {
                    return std::find(pendingRemovals.begin(), pendingRemovals.end(), e) != pendingRemovals.end();
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
        auto* audioSystem = ctx.getService<Audio::AudioSystem>();
        auto* animationSystem = ctx.getService<Animations::AnimationSystem>();
        auto* uiLayoutSystem = ctx.getService<UI::UILayoutSystem>();
        auto* uiInteractionSystem = ctx.getService<UI::UIInteractionSystem>();

        for ( auto& comp : entity->getComponents() ) {
            if ( auto* transformComp = dynamic_cast<TransformComponent*>(comp.get()) ) {
                if ( transformSystem ) {
                    transformSystem->registerComponent(transformComp);
                    // transformSystem->update(0.016f);
                }
            }
            else if ( auto* physComp = dynamic_cast<Physics::PhysicsBodyBaseComponent*>(comp.get()) ) {
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
            //else if ( auto* speakerComp = dynamic_cast<Audio::SpeakerComponent*>(comp.get()) ) {
            //    if ( audioSystem ) {
            //        audioSystem->registerComponent(speakerComp);
            //        // speakerComp->loadAudioAsset();
            //        if ( speakerComp->getPlayOnLoad() ) {
            //            speakerComp->playSound();
            //        }
            //    }
            //}
            else if ( auto* speakerComp = dynamic_cast<Audio::AudioComponent*>(comp.get()) ) {
                if ( audioSystem ) {
                    audioSystem->registerComponent(speakerComp);
                }
            }
            else if ( auto* uirenderComp = dynamic_cast<UI::UIRenderComponent*>(comp.get()) ) {
                if ( renderSystem ) {
                    renderSystem->registerComponent(uirenderComp);
                }
            }
            else if ( auto* uirenderComp = dynamic_cast<UI::UITextComponent*>(comp.get()) ) {
                if ( renderSystem ) {
                    renderSystem->registerComponent(uirenderComp);
                }
            }
            else if ( auto* uitransformComp = dynamic_cast<UI::UITransformComponent*>(comp.get()) ) {
                if ( uiLayoutSystem ) {
                    uiLayoutSystem->registerComponent(uitransformComp);
                }
            }
            else if ( auto* uiinteractionComp = dynamic_cast<UI::UIInteractionComponent*>(comp.get()) ) {
                if ( uiInteractionSystem ) {
                    uiInteractionSystem->registerComponent(uiinteractionComp);
                }
            }
            else if ( auto* AnimationComp = dynamic_cast<Animations::AnimationComponent*>(comp.get()) ) {
                //std::cout << "[Scene] Registering Animation Element " << std::endl;
                animationSystem->registerComponent(AnimationComp);
            }

            // add more as needed...
        }

        //Recursivvly Register Components
        auto transComp = entity->findComponent<TransformComponent>();
        if ( transComp ) {
            for ( TransformComponent* child : transComp->getChildren() ) {
                registerEntityComponents(child->getOwner());
            }
        }

    }

    void Scene::deregisterEntityComponents(Entity* entity) {
        if ( !entity ) return;

        // Recursivvly Register Components
        auto transComp = entity->findComponent<TransformComponent>();
        if ( transComp ) {
            for ( TransformComponent* child : transComp->getChildren() ) {
                deregisterEntityComponents(child->getOwner());
            }
        }

        auto* transformSystem = ctx.getService<TransformSystem>();
        auto* physicsSystem = ctx.getService<Physics::PhysicsSystem>();
        auto* renderSystem = ctx.getService<Graphics::RenderSystem>();
        auto* scriptingSystem = ctx.getService<Scripting::ScriptSystem>();
        auto* particleSystem = ctx.getService<Particles::ParticleSystem>();
        auto* audioSystem = ctx.getService<Audio::AudioSystem>();
        auto* animationSystem = ctx.getService<LaurelEye::Animations::AnimationSystem>();
        auto* uiLayoutSystem = ctx.getService<UI::UILayoutSystem>();
        auto* uiInteractionSystem = ctx.getService<UI::UIInteractionSystem>();

        for ( auto& comp : entity->getComponents() ) {
            if ( auto* transformComp = dynamic_cast<TransformComponent*>(comp.get()) ) {
                if ( transformSystem ) {
                    transformSystem->deregisterComponent(transformComp);
                }
            }
            else if ( auto* physComp = dynamic_cast<Physics::PhysicsBodyBaseComponent*>(comp.get()) ) {
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
            //else if ( auto* speakerComp = dynamic_cast<Audio::SpeakerComponent*>(comp.get()) ) {
            //    if ( audioSystem ) {
            //        audioSystem->deregisterComponent(speakerComp);
            //    }
            //    // BG music n jazz
            //    if ( speakerComp->getIsLooping() ) {
            //        speakerComp->stopSound(speakerComp->getAudioName());
            //    }
            //}
            else if ( auto* audioComp = dynamic_cast<Audio::AudioComponent*>(comp.get()) ) {
                if ( audioSystem ) {
                    audioComp->stopAll();
                    audioSystem->deregisterComponent(audioComp);
                }
            }
            else if ( auto* uirenderComp = dynamic_cast<UI::UIRenderComponent*>(comp.get()) ) {
                if ( renderSystem ) {
                    renderSystem->deregisterComponent(uirenderComp);
                }
            }
            else if ( auto* uitextComp = dynamic_cast<UI::UITextComponent*>(comp.get()) ) {
                if ( renderSystem ) {
                    renderSystem->deregisterComponent(uitextComp);
                }
            }
            else if ( auto* uitransformComp = dynamic_cast<UI::UITransformComponent*>(comp.get()) ) {
                if ( uiLayoutSystem ) {
                    uiLayoutSystem->deregisterComponent(uitransformComp);
                }
            }
            else if ( auto* uiinteractionComp = dynamic_cast<UI::UIInteractionComponent*>(comp.get()) ) {
                if ( uiInteractionSystem ) {
                    uiInteractionSystem->deregisterComponent(uiinteractionComp);
                }
            }
            else if ( auto* AnimationComp = dynamic_cast<Animations::AnimationComponent*>(comp.get()) ) {
                // std::cout << "[Scene] Registering Animation Element " << std::endl;
                if ( animationSystem ) {
                    animationSystem->deregisterComponent(AnimationComp);
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
