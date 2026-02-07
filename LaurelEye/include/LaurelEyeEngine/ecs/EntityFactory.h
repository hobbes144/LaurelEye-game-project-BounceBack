/// @file   EntityFactory.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-23-2025
/// @brief  An object factory that creates Entity instances, converting
/// JSON data into components and the overall Entity. Also features functionality
/// for populating a scene
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
#pragma once
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/math/VectorTemplated.h"

#include <memory>
#include <rapidjson/document.h>
#include <string>

namespace LaurelEye{
    class Scene;
    class EntityFactory {
    public:
        EntityFactory(EngineContext& ctx, const std::string& path)
            : context(ctx), assetPath(path) {}

        ///@brief Deserialize all entities in a scene JSON file, adding them to the given scene
        void populateSceneFromJson(Scene& scene, const rapidjson::Document& jsonDoc);

        ///@brief Deserialize one entity definition into a live entity
        Entity* createEntityFromJson(const rapidjson::Value& entityData);

        ///@brief Deserializes all components for a given entity, adding them to the entity
        void addComponentsFromJson(Entity& entity, const rapidjson::Value& componentsJson);

        /// @brief Given a path to a prefab, add it to the given scene
        /// @param scene 
        /// @param prefabPath 
        Entity* addPrefabToScene(Scene& scene, const std::string& prefabPath);
    private:
        EngineContext& context;
        const std::string& assetPath;

        /// @brief First pass — create all entity shells and add to scene.
        void createAndAddEntitiesToScene(Scene& scene,
                                         const rapidjson::GenericArray<true, rapidjson::Value>& entities,
                                         std::unordered_map<std::string, Entity*>& entityMap);

        /// @brief Second pass — link parent/child transforms.
        void resolveParentChildTransforms(Scene& scene,
                                          const rapidjson::GenericArray<true, rapidjson::Value>& entities,
                                          std::unordered_map<std::string, Entity*>& entityMap);

        // Per component setups
        void setupTransformComponent(Entity& entity, const rapidjson::Value& transformData);
        void setupRender3DComponent(Entity& entity, const rapidjson::Value& renderData);
        void setupCameraComponent(Entity& entity, const rapidjson::Value& cameraData);
        void setupLightComponent(Entity& entity, const rapidjson::Value& lightData);
        void setupPhysicsComponent(Entity& entity, const rapidjson::Value& physicsData);
        void setupScriptComponent(Entity& entity, const rapidjson::Value& scriptData);
        void setupParticleEmitterComponent(Entity& entity, const rapidjson::Value& emitterData);
        void setupSpeakerComponent(Entity& entity, const rapidjson::Value& speakerData);
        void setupDebugDrawComponent(Entity& entity, const rapidjson::Value& emitterData);
        void setupAnimationComponent(Entity& entity, const rapidjson::Value& animationData);
        void setupUIRenderComponent(Entity& entity, const rapidjson::Value& uirenderData);
        void setupUITransformComponent(Entity& entity, const rapidjson::Value& uitransformData);
        void setupUIInteractionComponent(Entity& entity, const rapidjson::Value& uiinteractionData);

        //Helpers
        Vector2 ReadVector2(const rapidjson::Value& vectorData);

    };
}

