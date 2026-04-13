#include "LaurelEyeEngine/ecs/EntityFactory.h"
#include "LaurelEyeEngine/audio/AudioSystem.h"
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/graphics/graphics_components/AmbientLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/CameraComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/DirectionalLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/PointLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/Renderable3DComponent.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include "LaurelEyeEngine/graphics/resources/RenderMesh.h"
#include "LaurelEyeEngine/io/AssetManager.h"
#include "LaurelEyeEngine/memory/MemoryManager.h"
#include "LaurelEyeEngine/particles/ParticleEmitterComponent.h"
#include "LaurelEyeEngine/physics/interfaces/PhysicsTypes.h"
#include "LaurelEyeEngine/physics/RigidBodyComponent.h"
#include "LaurelEyeEngine/physics/GhostBodyComponent.h"
#include "LaurelEyeEngine/scene/Scene.h"
#include "LaurelEyeEngine/scripting/ScriptComponent.h"
#include "LaurelEyeEngine/scripting/ScriptTypeRegistry.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UITransformComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UIRenderComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UITextComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UIInteractionComponent.h"

#include "LaurelEyeEngine/logging/EngineLog.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <iostream>
#include <LaurelEyeEngine/animation/resources/Animation.h>
#include <LaurelEyeEngine/animation/components/SkeletalAnimationComponent.h>
#include <LaurelEyeEngine/animation/AnimationManager.h>

namespace LaurelEye {

    void EntityFactory::populateSceneFromJson(Scene& scene, const rapidjson::Document& doc) {
        if ( !doc.HasMember("entities") || !doc["entities"].IsArray() ) {
            throw std::runtime_error("EntityFactory: Missing or invalid 'entities' array");
        }

        auto entities = doc["entities"].GetArray();
        std::unordered_map<std::string, Entity*> entityMap;

        // --- Pass 1: create all entities and add to scene ---
        createAndAddEntitiesToScene(scene, entities, entityMap);

        // --- Pass 2: resolve parent-child transforms ---
        resolveParentChildTransforms(scene, entities, entityMap);
    }

    Entity* EntityFactory::createEntityRecursive(
        Scene& scene,
        const rapidjson::Value& entityJson,
        Entity* parent,
        std::unordered_map<std::string, Entity*>& entityMap) {

        Entity* entity = createEntityFromJson(entityJson);

        entityMap[entity->getName()] = entity;
        scene.addEntity(entity);

        if ( parent ) {
            auto* childTransform = entity->findComponent<TransformComponent>();
            auto* parentTransform = parent->findComponent<TransformComponent>();

            if ( childTransform && parentTransform ) {
                childTransform->setParent(parentTransform);
            }
        }

        if ( entityJson.HasMember("Children") && entityJson["Children"].IsArray() ) {
            for ( const auto& childJson : entityJson["Children"].GetArray() ) {
                createEntityRecursive(scene, childJson, entity, entityMap);
            }
        }

        return entity;

    }

    Entity* EntityFactory::createEntityFromJson(const rapidjson::Value& entityJson) {
        if ( !entityJson.HasMember("name") || !entityJson["name"].IsString() )
            throw std::runtime_error("EntityFactory: Entity missing 'name' field");

        std::string name = entityJson["name"].GetString();
        auto* memManager = context.getService<MemoryManager>();
        if ( !memManager ) {
            throw std::runtime_error("EntityFactory: MemoryManager service not found in context");
        }
        auto entity = memManager->allocateMemory(name);

        if ( entityJson.HasMember("tags") && entityJson["tags"].IsArray() ) {
            const auto& tags = entityJson["tags"].GetArray();
            for ( const auto& tag : tags ) {
                if ( tag.IsString() ) {
                    entity->addTag(tag.GetString());
                }
            }
        }
        // Add components if present
        if ( entityJson.HasMember("components") && entityJson["components"].IsObject() ) {
            const auto& comps = entityJson["components"];
            addComponentsFromJson(*entity, comps);
        }
        else if ( entityJson.HasMember("prefab") && entityJson["prefab"].IsString() ) {
            const std::string& prefabPath = assetPath + entityJson["prefab"].GetString();
            auto* assetManager = context.getService<IO::AssetManager>();
            auto asset = assetManager->load(prefabPath);
            auto jsonAsset = std::dynamic_pointer_cast<IO::JsonAsset>(asset);
            if ( !jsonAsset ) {
                std::cerr << "EntityFactory: Could not parse prefab for entity";
            }
            entity = createEntityFromJson(jsonAsset->jsonDocument);
        }

        return entity;
    }

    void EntityFactory::addComponentsFromJson(Entity& entity, const rapidjson::Value& componentsJson) {
        for ( auto it = componentsJson.MemberBegin(); it != componentsJson.MemberEnd(); ++it ) {
            std::string compName = it->name.GetString();
            const auto& compData = it->value;

            if ( compName == "Transform" ) {
                setupTransformComponent(entity, compData);
            }
            else if ( compName == "Render3D" ) {
                setupRender3DComponent(entity, compData);
            }
            else if ( compName == "Camera" ) {
                setupCameraComponent(entity, compData);
            }
            else if ( compName == "Light" ) {
                setupLightComponent(entity, compData);
            }
            else if ( compName == "Physics" ) {
                setupPhysicsComponent(entity, compData);
            }
            else if ( compName == "Script" ) {
                setupScriptComponent(entity, compData);
            }
            else if ( compName == "ParticleEmitter" ) {
                setupParticleEmitterComponent(entity, compData);
            }
            //else if ( compName == "Speaker" ) {
            //    setupSpeakerComponent(entity, compData);
            //}
            else if ( compName == "Audio" ) {
                setupAudioComponent(entity, compData);
            }
            else if ( compName == "DebugDraw" ) {
                setupDebugDrawComponent(entity, compData);
            }
            else if ( compName == "UIRender" ) {
                setupUIRenderComponent(entity, compData);
            }
            else if ( compName == "UITransform" ) {
                setupUITransformComponent(entity, compData);
            }
            else if ( compName == "UIText" ) {
                setupUITextComponent(entity, compData);
            }
            else if ( compName == "UIInteraction" ) {
                setupUIInteractionComponent(entity, compData);
            }
            else if ( compName == "Animation" ) {
                setupAnimationComponent(entity, compData);
            }
            else {
                std::cerr << "Entity Factory: Unknown component type trying to be added to entity: " << entity.getName() << std::endl;
            }
        }
    }

    Entity* EntityFactory::addPrefabToScene(Scene& scene, const std::string& prefabPath) {
        std::string fullPath = assetPath + prefabPath;
        auto* assetManager = context.getService<IO::AssetManager>();
        auto asset = assetManager->load(fullPath);
        auto jsonAsset = std::dynamic_pointer_cast<IO::JsonAsset>(asset);
        if ( !jsonAsset ) {
            std::cerr << "EntityFactory: Could not parse prefab for entity";
        }
        std::unordered_map<std::string, Entity*> entityMap;
        auto entity = createEntityRecursive(scene, jsonAsset->jsonDocument, nullptr, entityMap);
        scene.addEntity(entity);
        return entity;
    }

    void EntityFactory::createAndAddEntitiesToScene(Scene& scene,
                                                    const rapidjson::GenericArray<true, rapidjson::Value>& entities,
                                                    std::unordered_map<std::string, Entity*>& entityMap) {
        for ( const auto& entityData : entities ) {
            createEntityRecursive(scene, entityData, nullptr, entityMap);
            //auto entity = createEntityFromJson(entityData);
            //entityMap[entity->getName()] = entity;
            //scene.addEntity(entity);
        }
    }

    void EntityFactory::resolveParentChildTransforms(
        Scene& scene,
        const rapidjson::GenericArray<true, rapidjson::Value>& entities,
        std::unordered_map<std::string, Entity*>& entityMap) {
        for ( const auto& entityData : entities ) {
            if ( !entityData.HasMember("name") || !entityData["name"].IsString() )
                continue;

            std::string name = entityData["name"].GetString();
            Entity* entity = entityMap[name];

            // Look for a parent reference
            if ( entityData.HasMember("parent") && entityData["parent"].IsString() ) {
                std::string parentName = entityData["parent"].GetString();

                if ( entityMap.contains(parentName) ) {
                    auto* childTransform = entity->findComponent<TransformComponent>();
                    auto* parentTransform = entityMap[parentName]->findComponent<TransformComponent>();

                    if ( childTransform && parentTransform ) {
                        childTransform->setParent(parentTransform);
                    }
                }
            }
        }
    }

    /// This region defines component setups for the entity
    /// Ideally this is done by the systems I think, but this is okay for now.
#pragma region Setup

    void EntityFactory::setupTransformComponent(Entity& entity, const rapidjson::Value& transformData) {
        auto* transform = entity.addComponent<TransformComponent>();
        if ( transformData.HasMember("position") ) {
            const auto& pos = transformData["position"];
            transform->setWorldPosition({pos[0].GetFloat(), pos[1].GetFloat(), pos[2].GetFloat()});
        }
        if ( transformData.HasMember("rotation") ) { // Quaternion
            const auto& rot = transformData["rotation"];
            if ( rot.IsArray() && rot.GetArray().Size() == 4 )
                transform->setWorldRotation({rot[0].GetFloat(), rot[1].GetFloat(), rot[2].GetFloat(), rot[3].GetFloat()});
            else
                transform->setWorldRotation(Vector3{rot[0].GetFloat(), rot[1].GetFloat(), rot[2].GetFloat()});
        }
        if ( transformData.HasMember("scale") ) {
            const auto& scale = transformData["scale"];
            transform->setWorldScale({scale[0].GetFloat(), scale[1].GetFloat(), scale[2].GetFloat()});
        }
    }

    void EntityFactory::setupRender3DComponent(Entity& entity, const rapidjson::Value& render3DData) {
        // Register renderer component

        // Make the Renderable Component
        std::unique_ptr<Graphics::Renderable3DComponent> renderComponent = std::make_unique<Graphics::Renderable3DComponent>();
        //---Mesh---
        // If a Mesh member is identified as a string
        if ( render3DData.HasMember("mesh") && render3DData["mesh"].IsObject() ) {
            const auto& mesh = render3DData["mesh"];
            if ( mesh.HasMember("file") && mesh["file"].IsString() ) {
                std::string meshPath = mesh["file"].GetString();
                auto asset = context.getService<IO::AssetManager>()->load(assetPath + meshPath);
                auto meshAsset = std::dynamic_pointer_cast<IO::MeshAsset>(asset);
                if ( !meshAsset ) {
                    throw((void)("EntityFactory::setupRender3DComponent - Asset '{}' is not a MeshAsset."), meshPath);
                    return;
                }
                renderComponent->SetMeshAsset(meshAsset);
            }

            else if ( mesh.HasMember("primitiveType") && mesh["primitiveType"].IsString() ) {
                std::string shapeType = mesh["primitiveType"].GetString();
                if ( shapeType == "Square" ) {
                    renderComponent->SetMeshPrimitiveType(Graphics::PrimitiveMeshType::Square);
                }
                else if ( shapeType == "Cube" ) {
                    renderComponent->SetMeshPrimitiveType(Graphics::PrimitiveMeshType::Cube);
                }
                else if ( shapeType == "Sphere" ) {
                    renderComponent->SetMeshPrimitiveType(Graphics::PrimitiveMeshType::Sphere);
                }
            }
        }

        //---Material---
        if ( render3DData.HasMember("material") && render3DData["material"].IsObject() ) {
            const auto& material = render3DData["material"];
            auto pMat = std::make_shared<Graphics::Material>();
            renderComponent->SetMaterial(pMat);

            // TODO: Make this a loop over all properties.
            if ( material.HasMember("texture") && material["texture"].IsString() ) {
                std::string texturePath = material["texture"].GetString();
                auto texAsset = context.getService<IO::AssetManager>()->load(assetPath + texturePath);
                auto imageAsset = std::dynamic_pointer_cast<IO::ImageAsset>(texAsset);
                renderComponent->SetImageAsset(imageAsset);
                renderComponent->GetMaterial()->setProperty<int>("useTexture", 1);
            }
            else {
                renderComponent->GetMaterial()->setProperty<int>("useTexture", 0);
            }

            if ( material.HasMember("mainTextureScale") && material["mainTextureScale"].IsArray() ) {
                const auto& scale = material["mainTextureScale"].GetArray();
                renderComponent->GetMaterial()->setProperty<Vector2>(
                    "mainTextureScale",
                    Vector2(scale[0].GetFloat(), scale[1].GetFloat()));
            }
            else {
                renderComponent->GetMaterial()->setProperty<Vector2>("mainTextureScale", Vector2(1.0f));
            }

            if ( material.HasMember("diffuse") && material["diffuse"].IsArray() ) {
                const auto& diff = material["diffuse"].GetArray();
                renderComponent->GetMaterial()->setProperty<Vector3>("diffuse", Vector3(diff[0].GetFloat(), diff[1].GetFloat(), diff[2].GetFloat()));
            }
            if ( material.HasMember("specular") && material["specular"].IsArray() ) {
                const auto& spec = material["specular"].GetArray();
                renderComponent->GetMaterial()->setProperty<Vector3>("specular", Vector3(spec[0].GetFloat(), spec[1].GetFloat(), spec[2].GetFloat()));
            }
            if ( material.HasMember("shininess") && material["shininess"].IsFloat() ) {
                float shininess = material["shininess"].GetFloat();
                renderComponent->GetMaterial()->setProperty<float>("shininess", shininess);
            }
        }

        entity.addComponent<Graphics::Renderable3DComponent>(std::move(renderComponent));
    }

    void EntityFactory::setupCameraComponent(Entity& entity, const rapidjson::Value& cameraData) {
        std::unique_ptr<Graphics::CameraComponent> cameraComponent = std::make_unique<Graphics::CameraComponent>();

        // cameraComponent->setPositionRotation(
        //     Vector3(-35.f, 15.f, 0.f),
        //     Matrix4::lookAt(Vector3(-50.f, 15.f, 0.f), Vector3(0.f)).toQuaternion());
        cameraComponent->setPerspectiveProjection(45.0f * 3.14159f / 180.0f, 1280.f / 720.f, 0.1f, 5000.0f);
        entity.addComponent<Graphics::CameraComponent>(std::move(cameraComponent));
    }

    void EntityFactory::setupLightComponent(Entity& entity, const rapidjson::Value& lightData) {
        if ( !lightData.HasMember("type") || !lightData["type"].IsString() ) {
            std::cerr << "EntityFactory: Light component missing 'type' field on entity: "
                      << entity.getName() << std::endl;
            return;
        }

        std::string lightType = lightData["type"].GetString();

        if ( lightType == "Directional" ) {
            Vector3 direction(0.0f, -1.0f, 0.0f);
            float intensity = 1.0f;
            Vector3 color(1.0f, 1.0f, 1.0f);

            if ( lightData.HasMember("direction") && lightData["direction"].IsArray() ) {
                const auto& dir = lightData["direction"].GetArray();
                if ( dir.Size() >= 3 )
                    direction = Vector3(dir[0].GetFloat(), dir[1].GetFloat(), dir[2].GetFloat()).normalized();
            }

            if ( lightData.HasMember("intensity") && lightData["intensity"].IsNumber() ) {
                intensity = lightData["intensity"].GetFloat();
            }

            if ( lightData.HasMember("color") && lightData["color"].IsArray() ) {
                const auto& col = lightData["color"].GetArray();
                if ( col.Size() >= 3 )
                    color = Vector3(col[0].GetFloat(), col[1].GetFloat(), col[2].GetFloat());
            }

            Graphics::DirectionalLight DLightData{
                direction,
                intensity,
                color};

            entity.addComponent<Graphics::DirectionalLightComponent>(DLightData);
        }

        else if ( lightType == "Point" ) {
            Vector3 position(0.0f, 0.0f, 0.0f);
            float intensity = 1.0f;
            Vector3 color(1.0f, 1.0f, 1.0f);
            float range = 10.0f;

            if ( lightData.HasMember("position") && lightData["position"].IsArray() ) {
                const auto& pos = lightData["position"].GetArray();
                if ( pos.Size() >= 3 )
                    position = Vector3(pos[0].GetFloat(), pos[1].GetFloat(), pos[2].GetFloat());
            }

            if ( lightData.HasMember("intensity") && lightData["intensity"].IsNumber() )
                intensity = lightData["intensity"].GetFloat();

            if ( lightData.HasMember("color") && lightData["color"].IsArray() ) {
                const auto& col = lightData["color"].GetArray();
                if ( col.Size() >= 3 )
                    color = Vector3(col[0].GetFloat(), col[1].GetFloat(), col[2].GetFloat());
            }

            if ( lightData.HasMember("range") && lightData["range"].IsNumber() )
                range = lightData["range"].GetFloat();

            Graphics::PointLight PLightData{position, intensity, color, range};
            entity.addComponent<Graphics::PointLightComponent>(PLightData);
        }

        // -------------------------------
        // Ambient Light
        // -------------------------------
        else if ( lightType == "Ambient" ) {
            Vector3 color(1.0f, 1.0f, 1.0f);
            float intensity = 0.5f;

            if ( lightData.HasMember("color") && lightData["color"].IsArray() ) {
                const auto& col = lightData["color"].GetArray();
                if ( col.Size() >= 3 )
                    color = Vector3(col[0].GetFloat(), col[1].GetFloat(), col[2].GetFloat());
            }

            if ( lightData.HasMember("intensity") && lightData["intensity"].IsNumber() )
                intensity = lightData["intensity"].GetFloat();

            Graphics::AmbientLight ALightData{color, intensity};
            entity.addComponent<Graphics::AmbientLightComponent>(ALightData);
        }
    }

    void EntityFactory::setupPhysicsComponent(Entity& entity, const rapidjson::Value& physicsData) {
        using namespace LaurelEye::Physics;
        PhysicsBodyData data;

        // --- BodyType ---
        if ( physicsData.HasMember("type") && physicsData["type"].IsString() ) {
            std::string typeStr = physicsData["type"].GetString();
            if ( typeStr == "Static" ) data.type = BodyType::Static;
            else if ( typeStr == "Dynamic" ) data.type = BodyType::Dynamic;
            else if ( typeStr == "Kinematic" ) data.type = BodyType::Kinematic;
            else if ( typeStr == "Ghost" ) data.type = BodyType::Ghost;
        }

        // --- Collision Shape ---
        if ( physicsData.HasMember("shapeDefinition") && physicsData["shapeDefinition"].IsObject() ) {
            const auto& shape = physicsData["shapeDefinition"];
            if ( shape.HasMember("type") && shape["type"].IsString() ) {
                std::string shapeStr = shape["type"].GetString();
                if ( shapeStr == "Box" ) data.shapeDefinition.type = CollisionShapePhys::ShapeType::Box;
                else if ( shapeStr == "Sphere" ) data.shapeDefinition.type = CollisionShapePhys::ShapeType::Sphere;
                else if ( shapeStr == "Capsule" ) data.shapeDefinition.type = CollisionShapePhys::ShapeType::Capsule;
                else if ( shapeStr == "Mesh" ) data.shapeDefinition.type = CollisionShapePhys::ShapeType::Mesh;
            }

            if ( shape.HasMember("size") && shape["size"].IsObject() ) {
                const auto& s = shape["size"];
                data.shapeDefinition.size.x = s.HasMember("x") ? s["x"].GetFloat() : 1.0f;
                data.shapeDefinition.size.y = s.HasMember("y") ? s["y"].GetFloat() : 1.0f;
                data.shapeDefinition.size.z = s.HasMember("z") ? s["z"].GetFloat() : 1.0f;
            }

            if ( shape.HasMember("radius") ) data.shapeDefinition.radius = shape["radius"].GetFloat();
            if ( shape.HasMember("height") ) data.shapeDefinition.height = shape["height"].GetFloat();
        }

        // --- Mass and physical properties ---
        if ( physicsData.HasMember("mass") ) data.mass = physicsData["mass"].GetFloat();
        if ( physicsData.HasMember("linearVelocity") && physicsData["linearVelocity"].IsObject() ) {
            const auto& i = physicsData["linearVelocity"];
            data.linearVelocity.x = i.HasMember("x") ? i["x"].GetFloat() : 0.0f;
            data.linearVelocity.y = i.HasMember("y") ? i["y"].GetFloat() : 0.0f;
            data.linearVelocity.z = i.HasMember("z") ? i["z"].GetFloat() : 0.0f;
        }
        if ( physicsData.HasMember("angularVelocity") && physicsData["angularVelocity"].IsObject() ) {
            const auto& i = physicsData["angularVelocity"];
            data.angularVelocity.x = i.HasMember("x") ? i["x"].GetFloat() : 0.0f;
            data.angularVelocity.y = i.HasMember("y") ? i["y"].GetFloat() : 0.0f;
            data.angularVelocity.z = i.HasMember("z") ? i["z"].GetFloat() : 0.0f;
        }
        if ( physicsData.HasMember("inertia") && physicsData["inertia"].IsObject() ) {
            const auto& i = physicsData["inertia"];
            data.inertia.x = i.HasMember("x") ? i["x"].GetFloat() : 0.0f;
            data.inertia.y = i.HasMember("y") ? i["y"].GetFloat() : 0.0f;
            data.inertia.z = i.HasMember("z") ? i["z"].GetFloat() : 0.0f;
        }
        if ( physicsData.HasMember("centerOfMass") && physicsData["centerOfMass"].IsObject() ) {
            const auto& c = physicsData["centerOfMass"];
            data.centerOfMass.x = c.HasMember("x") ? c["x"].GetFloat() : 0.0f;
            data.centerOfMass.y = c.HasMember("y") ? c["y"].GetFloat() : 0.0f;
            data.centerOfMass.z = c.HasMember("z") ? c["z"].GetFloat() : 0.0f;
        }
        if ( physicsData.HasMember("rotationOfCenter") && physicsData["rotationOfCenter"].IsObject() ) {
            const auto& c = physicsData["rotationOfCenter"];
            data.rotationOfCenter.x = c.HasMember("x") ? c["x"].GetFloat() : 0.0f;
            data.rotationOfCenter.y = c.HasMember("y") ? c["y"].GetFloat() : 0.0f;
            data.rotationOfCenter.z = c.HasMember("z") ? c["z"].GetFloat() : 0.0f;
        }

        if ( physicsData.HasMember("linearDamping") ) data.linearDamping = physicsData["linearDamping"].GetFloat();
        if ( physicsData.HasMember("angularDamping") ) data.angularDamping = physicsData["angularDamping"].GetFloat();
        if ( physicsData.HasMember("friction") ) data.friction = physicsData["friction"].GetFloat();
        if ( physicsData.HasMember("restitution") ) data.restitution = physicsData["restitution"].GetFloat();

        if ( physicsData.HasMember("frozenTranslations") && physicsData["frozenTranslations"].IsObject() ) {
            const auto& f = physicsData["frozenTranslations"];
            data.freezeTranslation.x = f.HasMember("x") ? f["x"].GetBool() : false;
            data.freezeTranslation.y = f.HasMember("y") ? f["y"].GetBool() : false;
            data.freezeTranslation.z = f.HasMember("z") ? f["z"].GetBool() : false;
        }
        if ( physicsData.HasMember("frozenRotations") && physicsData["frozenRotations"].IsObject() ) {
            const auto& f = physicsData["frozenRotations"];
            data.freezeRotation.x = f.HasMember("x") ? f["x"].GetBool() : false;
            data.freezeRotation.y = f.HasMember("y") ? f["y"].GetBool() : false;
            data.freezeRotation.z = f.HasMember("z") ? f["z"].GetBool() : false;
        }

        if ( physicsData.HasMember("collisionGroup") ) data.collisionGroup = physicsData["collisionGroup"].GetUint();
        if ( physicsData.HasMember("collisionMask") ) data.collisionMask = physicsData["collisionMask"].GetUint();

        if ( physicsData.HasMember("useCCD") ) data.useCCD = physicsData["useCCD"].GetBool();
        if ( physicsData.HasMember("ccdMotionThreshold") ) data.ccdMotionThreshold = physicsData["ccdMotionThreshold"].GetFloat();
        if ( physicsData.HasMember("ccdSweptSphereRadius") ) data.ccdSweptSphereRadius = physicsData["ccdSweptSphereRadius"].GetFloat();

        if ( physicsData.HasMember("gravityScale") ) data.gravityScale = physicsData["gravityScale"].GetFloat();

        // --- Assign to component ---
        if ( data.type == BodyType::Dynamic ||
             data.type == BodyType::Static ||
             data.type == BodyType::Kinematic ) {
            auto* physicsComp = entity.addComponent<Physics::RigidBodyComponent>(data);
        }
        else if ( data.type == BodyType::Ghost ) {
            auto* physicsComp = entity.addComponent<Physics::GhostBodyComponent>(data);
        }
        else {
            std::cerr << "[EntityFactory::setupPhysicsComponent] INVALID BODY TYPE\n";
        }
    }

    void EntityFactory::setupScriptComponent(Entity& entity, const rapidjson::Value& scriptData) {
        // Register script component
        if ( !scriptData.HasMember("path") || !scriptData["path"].IsString() ) {
            LE_DEBUG_ASSERT("Entity Factory", false,"EntityFactory: Script component missing 'path' for entity: " << entity.getName());
            return;
        }

        std::string relPath = scriptData["path"].GetString();
        std::string resolvedPath = assetPath + relPath;

        auto scriptComp = entity.addComponent<Scripting::ScriptComponent>(resolvedPath);

        //Serialized Data Values
        if ( scriptData.HasMember("data") && scriptData["data"].IsArray() ) {

            const auto& dataArray = scriptData["data"];

            for ( const auto& entry : dataArray.GetArray() ) {
                if ( !entry.IsObject() ||
                     !entry.HasMember("name") || !entry.HasMember("type") ||
                     !entry["name"].IsString() || !entry["type"].IsString() ) {
                    LE_DEBUG_ERROR("EntityFactory", "Invalid Serialized Data Value Structure");
                    continue;
                }

                std::string name = entry["name"].GetString();
                std::string type = entry["type"].GetString();

                const rapidjson::Value& value = entry["value"];

                auto parsed = Scripting::ScriptTypeRegistry::parse(type, value);

                LE_DEBUG_ASSERT(
                    "EntityFactory",
                    parsed.has_value(),
                    "Unknown script type: " << type);

                scriptComp->setSerializedValue(name, *parsed);
            }

        }

    }

    void EntityFactory::setupParticleEmitterComponent(Entity& entity, const rapidjson::Value& emitterData) {

        Particles::ParticleEmitterData data;

        // Scalars
        if ( emitterData.HasMember("emissionRate") ) data.emissionRate = emitterData["emissionRate"].GetFloat();
        if ( emitterData.HasMember("spreadAngle") ) data.spreadAngle = emitterData["spreadAngle"].GetFloat();
        if ( emitterData.HasMember("particleLifetime") ) data.particleLifetime = emitterData["particleLifetime"].GetFloat();
        if ( emitterData.HasMember("lifetimeVariation") ) data.lifetimeVariation = emitterData["lifetimeVariation"].GetFloat();
        if ( emitterData.HasMember("initialSpeed") ) data.initialSpeed = emitterData["initialSpeed"].GetFloat();
        if ( emitterData.HasMember("speedVariation") ) data.speedVariation = emitterData["speedVariation"].GetFloat();
        if ( emitterData.HasMember("startSize") ) data.startSize = emitterData["startSize"].GetFloat();
        if ( emitterData.HasMember("endSize") ) data.endSize = emitterData["endSize"].GetFloat();
        if ( emitterData.HasMember("sizeVariation") ) data.sizeVariation = emitterData["sizeVariation"].GetFloat();
        if ( emitterData.HasMember("usePhysics") ) data.usePhysics = emitterData["usePhysics"].GetBool();

        // Colors
        if ( emitterData.HasMember("startColor") && emitterData["startColor"].IsArray() ) {
            const auto& col = emitterData["startColor"];
            data.startColor = Vector4();
            data.startColor[0] = col[0].GetFloat();
            data.startColor[1] = col[1].GetFloat();
            data.startColor[2] = col[2].GetFloat();
            data.startColor[3] = col[3].GetFloat();
        }

        if ( emitterData.HasMember("endColor") && emitterData["endColor"].IsArray() ) {
            const auto& col = emitterData["endColor"];
            data.endColor = Vector4();
            data.endColor[0] = col[0].GetFloat();
            data.endColor[1] = col[1].GetFloat();
            data.endColor[2] = col[2].GetFloat();
            data.endColor[3] = col[3].GetFloat();
        }

        // Create and initialize component
        auto emitter = entity.addComponent<Particles::ParticleEmitterComponent>();
        emitter->GetEmitterData() = data;

        if ( emitterData.HasMember("maxParticles") ) {
            emitter->SetMaxParticles(emitterData["maxParticles"].GetUint());
        }
    }

    //void EntityFactory::setupSpeakerComponent(Entity& entity, const rapidjson::Value& speakerData) {
    //    auto audioSystem = context.getService<Audio::AudioSystem>();
    //    std::string audioName = "";
    //    std::string audioPath = "";
    //    float volume = 0.0f;
    //    bool is3D = false;
    //    bool isLooping = false;
    //    bool playOnLoad = false;

    //    if ( speakerData.HasMember("audioName") && speakerData["audioName"].IsString() ) {
    //        audioName = speakerData["audioName"].GetString();
    //    }
    //    if ( speakerData.HasMember("audioPath") && speakerData["audioPath"].IsString() ) {
    //        audioPath = speakerData["audioPath"].GetString();
    //    }
    //    else {
    //        std::cerr << "Audio path invalid for entity: " << entity.getName() << std::endl;
    //        return;
    //    }
    //    if ( speakerData.HasMember("volume") && speakerData["volume"].IsFloat() ) {
    //        volume = speakerData["volume"].GetFloat();
    //    }
    //    if ( speakerData.HasMember("3D") && speakerData["3D"].IsBool() ) {
    //        is3D = speakerData["3D"].GetBool();
    //    }
    //    if ( speakerData.HasMember("loop") && speakerData["loop"].IsBool() ) {
    //        isLooping = speakerData["loop"].GetBool();
    //    }
    //    if ( speakerData.HasMember("playOnLoad") && speakerData["playOnLoad"].IsBool() ) {
    //        playOnLoad = speakerData["playOnLoad"].GetBool();
    //    }
    //    auto am = audioSystem->getAudioManager();
    //    Audio::SpeakerComponent* sp = entity.addComponent<Audio::SpeakerComponent>();
    //    sp->setAudioManager(am);
    //    sp->setAudioName(audioName);
    //    sp->setPlayOnLoad(playOnLoad);
    //    sp->setIsLooping(isLooping);

    //    sp->createAudioAsset(assetPath + audioPath, volume, is3D, isLooping);
    //    sp->loadAudioAsset();

    //    assert(sp != nullptr);
    //}

    void EntityFactory::setupAudioComponent(Entity& entity, const rapidjson::Value& audioData) {
        if ( !audioData.HasMember("sounds") || !audioData["sounds"].IsArray() ) {
            std::cerr << "Audio data missing 'sounds' array for entity: "
                      << entity.getName() << std::endl;
            return;
        }

        auto audioSystem = context.getService<Audio::AudioSystem>();
        auto audioManager = audioSystem->getAudioManager();

        // Create component
        Audio::AudioComponent* audioComp = entity.addComponent<Audio::AudioComponent>(audioManager);

        const auto& soundsArray = audioData["sounds"].GetArray();

        for ( const auto& s : soundsArray ) {
            if ( !s.HasMember("id") || !s.HasMember("path") || !s["id"].IsString() || !s["path"].IsString() ) {
                std::cerr << "Invalid sound entry in entity: " << entity.getName() << std::endl;
                continue;
            }

            std::string id = s["id"].GetString();
            std::string path = s["path"].GetString();
            float volume = s.HasMember("volume") ? s["volume"].GetFloat() : 1.0f;
            float fadeSpeed = s.HasMember("fadeSpeed") ? s["fadeSpeed"].GetFloat() : 1.0f;
            bool looping = s.HasMember("loop") ? s["loop"].GetBool() : false;
            bool playOnLoad = s.HasMember("playOnLoad") ? s["playOnLoad"].GetBool() : false;

            // Determine sound type
            Audio::SoundType type = Audio::SoundType::SFX2D;
            if ( s.HasMember("type") && s["type"].IsString() ) {
                std::string t = s["type"].GetString();
                if ( t == "BGM" ) type = Audio::SoundType::BGM;
                else if ( t == "SFX3D" ) type = Audio::SoundType::SFX3D;
                else if ( t == "UI" ) type = Audio::SoundType::UI;
            }

            // Register sound in component
            audioComp->addSound(id, path, type, volume, fadeSpeed, looping);

            // Play immediately if requested
            if ( playOnLoad ) {
                Audio::SoundHandle handle = audioManager->createSound(path, type == Audio::SoundType::SFX3D, looping);
                audioComp->play(id);
            }
        }
    }

    void EntityFactory::setupUITransformComponent(Entity& entity, const rapidjson::Value& uitransformData) {
        UI::UITransformData t;

        if ( uitransformData.HasMember("anchorMin") ) {
            const auto& anchorMin = uitransformData["anchorMin"];
            t.anchorMin = {anchorMin[0].GetFloat(), anchorMin[1].GetFloat()};
        }
        if ( uitransformData.HasMember("anchorMax") ) {
            const auto& anchorMax = uitransformData["anchorMax"];
            t.anchorMax = {anchorMax[0].GetFloat(), anchorMax[1].GetFloat()};
        }
        if ( uitransformData.HasMember("pivot") ) {
            const auto& pivot = uitransformData["pivot"];
            t.pivot = {pivot[0].GetFloat(), pivot[1].GetFloat()};
        }
        if ( uitransformData.HasMember("offset") ) {
            const auto& offset = uitransformData["offset"];
            t.localOffset = {offset[0].GetFloat(), offset[1].GetFloat()};
        }
        if ( uitransformData.HasMember("size") ) {
            const auto& size = uitransformData["size"];
            t.size = {size[0].GetFloat(), size[1].GetFloat()};
        }
        std::unique_ptr<UI::UITransformComponent> uitransformComponent = std::make_unique<UI::UITransformComponent>(t);
        entity.addComponent(std::move(uitransformComponent));
    }

    void EntityFactory::setupUIRenderComponent(Entity& entity, const rapidjson::Value& uirenderData) {
        std::unique_ptr<UI::UIRenderComponent> uirenderComponent = std::make_unique<UI::UIRenderComponent>();

        // ALWAYS create material
        auto pMat = std::make_shared<Graphics::Material>();
        uirenderComponent->SetMaterial(pMat);

        //---Material---
        // Default
        pMat->setProperty<int>("useTexture", 0);
        pMat->setProperty<Vector2>("mainTextureScale", Vector2(1.0f));
        pMat->setProperty<Vector3>("diffuse", Vector3(1, 1, 1));
        pMat->setProperty<Vector3>("specular", Vector3(0, 0, 0));
        pMat->setProperty<float>("shininess", 1.0f);
        pMat->setProperty<int>("layer", 0);
        // JSON Override
        if ( uirenderData.HasMember("material") && uirenderData["material"].IsObject() ) {
            const auto& material = uirenderData["material"];

            // TODO: Make this a loop over all properties.
            if ( material.HasMember("texture") && material["texture"].IsString() ) {
                std::string texturePath = material["texture"].GetString();
                auto texAsset = context.getService<IO::AssetManager>()->load(assetPath + texturePath);
                auto imageAsset = std::dynamic_pointer_cast<IO::ImageAsset>(texAsset);
                uirenderComponent->SetImageAsset(imageAsset);
                uirenderComponent->GetMaterial()->setProperty<int>("useTexture", 1);
            }
            else {
                uirenderComponent->GetMaterial()->setProperty<int>("useTexture", 0);
            }

            if ( material.HasMember("mainTextureScale") && material["mainTextureScale"].IsArray() ) {
                const auto& scale = material["mainTextureScale"].GetArray();
                uirenderComponent->GetMaterial()->setProperty<Vector2>(
                    "mainTextureScale",
                    Vector2(scale[0].GetFloat(), scale[1].GetFloat()));
            }
            else {
                uirenderComponent->GetMaterial()->setProperty<Vector2>("mainTextureScale", Vector2(1.0f));
            }

            if ( material.HasMember("diffuse") && material["diffuse"].IsArray() ) {
                const auto& diff = material["diffuse"].GetArray();
                uirenderComponent->GetMaterial()->setProperty<Vector3>("diffuse", Vector3(diff[0].GetFloat(), diff[1].GetFloat(), diff[2].GetFloat()));
            }
            if ( material.HasMember("specular") && material["specular"].IsArray() ) {
                const auto& spec = material["specular"].GetArray();
                uirenderComponent->GetMaterial()->setProperty<Vector3>("specular", Vector3(spec[0].GetFloat(), spec[1].GetFloat(), spec[2].GetFloat()));
            }
            if ( material.HasMember("shininess") && material["shininess"].IsFloat() ) {
                float shininess = material["shininess"].GetFloat();
                uirenderComponent->GetMaterial()->setProperty<float>("shininess", shininess);
            }
            if ( material.HasMember("layer") && material["layer"].IsInt() ) {
                int layer = material["layer"].GetInt();
                uirenderComponent->GetMaterial()->setProperty<float>("layer", layer);
            }
        }

        // Color
        if ( uirenderData.HasMember("color") && uirenderData["color"].IsArray() ) {
            const auto& color = uirenderData["color"].GetArray();
            if ( color.Size() >= 4 ) {
                float r = color[0].GetFloat() <= 1 ? color[0].GetFloat() : 1.0;
                float g = color[1].GetFloat() <= 1 ? color[1].GetFloat() : 1.0;
                float b = color[2].GetFloat() <= 1 ? color[2].GetFloat() : 1.0;
                float a = color[3].GetFloat() <= 1 ? color[3].GetFloat() : 1.0;
                Vector4 c = Vector4(r, g, b, a);
                uirenderComponent->SetColor(c);
            }
            else {
                std::cerr << "Color is Not a Vector4\n";
            }
        }

        //Transparency
        if ( uirenderData.HasMember("transparency") && uirenderData["transparency"].IsFloat() ) {
            float trans = uirenderData["transparency"].GetFloat();
            uirenderComponent->SetTransparency(trans);
        }

        entity.addComponent(std::move(uirenderComponent));
    }



    void EntityFactory::setupUIInteractionComponent(Entity& entity, const rapidjson::Value& uiinteractionData) {
        std::unique_ptr<UI::UIInteractionComponent> uiinteractionComponent = std::make_unique<UI::UIInteractionComponent>();

        // ---- Core Flags ----
        if ( uiinteractionData.HasMember("isInteractable") &&
             uiinteractionData["isInteractable"].IsBool() ) {
            uiinteractionComponent->setInteractable(uiinteractionData["isInteractable"].GetBool());
        }

        if ( uiinteractionData.HasMember("blockRaycast") &&
             uiinteractionData["blockRaycast"].IsBool() ) {
            uiinteractionComponent->setBlockingRaycasts(uiinteractionData["blockRaycast"].GetBool());
        }

        // ---- Interaction Types ----
        if ( uiinteractionData.HasMember("receiveHover") &&
             uiinteractionData["receiveHover"].IsBool() ) {
            uiinteractionComponent->setReceiveHover(uiinteractionData["receiveHover"].GetBool());
        }

        if ( uiinteractionData.HasMember("receiveClick") &&
             uiinteractionData["receiveClick"].IsBool() ) {
            uiinteractionComponent->setReceiveClick(uiinteractionData["receiveClick"].GetBool());
        }

        if ( uiinteractionData.HasMember("receiveDrag") &&
             uiinteractionData["receiveDrag"].IsBool() ) {
            uiinteractionComponent->setReceiveDrag(uiinteractionData["receiveDrag"].GetBool());
        }

        if ( uiinteractionData.HasMember("dragThreshold") &&
             uiinteractionData["dragThreshold"].IsFloat() ) {
            uiinteractionComponent->setDragThreshold(uiinteractionData["dragThreshold"].GetFloat());
        }

        if ( uiinteractionData.HasMember("focusable") &&
             uiinteractionData["focusable"].IsBool() ) {
            uiinteractionComponent->setFocusable(uiinteractionData["focusable"].GetBool());
        }

        // ---- Priority ----
        if ( uiinteractionData.HasMember("interactionPriority") &&
             uiinteractionData["interactionPriority"].IsInt() ) {
            uiinteractionComponent->setInteractionPriority(uiinteractionData["interactionPriority"].GetInt());
        }

        entity.addComponent(std::move(uiinteractionComponent));
    }

    void EntityFactory::setupDebugDrawComponent(Entity& entity, const rapidjson::Value& emitterData) {
    }

    void EntityFactory::setupUITextComponent(Entity& entity, const rapidjson::Value& uitextData) {
        std::unique_ptr<UI::UITextComponent> textComponent = std::make_unique<UI::UITextComponent>();

        // ALWAYS create material (same pattern as UI quad)
        auto pMat = std::make_shared<Graphics::Material>();
        textComponent->SetMaterial(pMat);

        // --- Material Defaults ---
        pMat->setProperty<Vector2>("mainTextureScale", Vector2(1.0f));
        pMat->setProperty<Vector3>("diffuse", Vector3(1, 1, 1));
        pMat->setProperty<Vector3>("specular", Vector3(0, 0, 0));
        pMat->setProperty<float>("shininess", 1.0f);
        pMat->setProperty<int>("layer", 0);

        // Text shader typically doesn't use this toggle, but keep consistent
        pMat->setProperty<int>("useTexture", 1);

        // --- Font ---
        if ( uitextData.HasMember("font") && uitextData["font"].IsString() ) {
            std::string fontPath = uitextData["font"].GetString();

            textComponent->SetFontName(fontPath);

        }

        // --- Text Content ---
        if ( uitextData.HasMember("text") && uitextData["text"].IsString() ) {
            textComponent->SetText(uitextData["text"].GetString());
        }

        // --- Font Size ---
        if ( uitextData.HasMember("fontSize") && uitextData["fontSize"].IsInt()) {
            textComponent->SetFontSize(uitextData["fontSize"].GetInt());
        }

        // --- Color ---
        if ( uitextData.HasMember("color") && uitextData["color"].IsArray() ) {
            const auto& color = uitextData["color"].GetArray();
            if ( color.Size() >= 4 ) {
                float r = color[0].GetFloat() <= 1 ? color[0].GetFloat() : 1.0f;
                float g = color[1].GetFloat() <= 1 ? color[1].GetFloat() : 1.0f;
                float b = color[2].GetFloat() <= 1 ? color[2].GetFloat() : 1.0f;
                float a = color[3].GetFloat() <= 1 ? color[3].GetFloat() : 1.0f;
                textComponent->SetColor(Vector4(r, g, b, a));
            }
            else {
                std::cerr << "[UITextComponent] Color is not a Vector4\n";
            }
        }

        // --- Transparency ---
        if ( uitextData.HasMember("transparency") && uitextData["transparency"].IsFloat() ) {
            textComponent->SetTransparency(uitextData["transparency"].GetFloat());
        }

        // --- Alignment ---
        if ( uitextData.HasMember("alignment") && uitextData["alignment"].IsString() ) {
            std::string align = uitextData["alignment"].GetString();

            if ( align == "left" ) {
                textComponent->SetAlignment(UI::TextAlignment::Left);
            }
            else if ( align == "center" ) {
                textComponent->SetAlignment(UI::TextAlignment::Center);
            }
            else if ( align == "right" ) {
                textComponent->SetAlignment(UI::TextAlignment::Right);
            }
        }

        // --- Wrapping ---
        if ( uitextData.HasMember("wrap") && uitextData["wrap"].IsBool() ) {
            textComponent->SetWrap(uitextData["wrap"].GetBool());
        }

        if ( uitextData.HasMember("maxWidth") && uitextData["maxWidth"].IsFloat() ) {
            textComponent->SetMaxWidth(uitextData["maxWidth"].GetFloat());
        }

        entity.addComponent(std::move(textComponent));
    }

    void EntityFactory::setupAnimationComponent(Entity& entity, const rapidjson::Value& animationData) {
        if ( !animationData.IsObject() )
            return;

        assert((std::cout << "Setup AnimationComponent\n", true));

        // Create the component
        auto animComponent = std::make_unique<Animations::SkeletalAnimationComponent>();

        // --- Animation Type ---
        if ( animationData.HasMember("type") && animationData["type"].IsString() ) {
            std::string typeStr = animationData["type"].GetString();

            if ( typeStr == "Basic" )
                animComponent->type = Animations::Animation::Type::Basic;
            else if ( typeStr == "Skeletal" )
                animComponent->type = Animations::Animation::Type::Skeletal;
            else
                animComponent->type = Animations::Animation::Type::Skeletal;
                // Add more if needed
        }
        //default to be Skeletal/ for now
        else
            animComponent->type = Animations::Animation::Type::Skeletal;

        // --- Load Animation Asset ---
        // temp for now, mutiply Animation loading
        if ( animationData.HasMember("file") ) {
            auto& fileNode = animationData["file"];

            std::vector<std::string> animPaths;

            // Case 1: Single string
            if ( fileNode.IsString() ) {
                animPaths.push_back(fileNode.GetString());
            }
            // Case 2: Array of strings
            else if ( fileNode.IsArray() ) {
                for ( auto& v : fileNode.GetArray() ) {
                    if ( v.IsString() ) {
                        animPaths.push_back(v.GetString());
                    }
                }
            }
            else {
                throw std::runtime_error("EntityFactory::setupAnimationComponent - 'file' must be string or array of strings.");
            }

            // Process each animation file
            for ( const auto& animPath : animPaths ) {
                auto asset = context.getService<IO::AssetManager>()->load(assetPath + animPath);
                auto skinnedMeshAsset = std::dynamic_pointer_cast<IO::SkinnedMeshAsset>(asset);
                auto animAsset = std::dynamic_pointer_cast<IO::AnimationAsset>(skinnedMeshAsset->animation);

                if ( !animAsset ) {
                    throw std::runtime_error("EntityFactory::setupAnimationComponent - Asset '" + animPath + "' is not an AnimationAsset.");
                }

                assert((std::cout << "Loaded asset " + assetPath + animPath + "\n", true));

                auto animManager = context.getService<Animations::AnimationManager>();
                Animations::AnimationHandle handle = animManager->createAnimation(animAsset.get());

                // set last animation as default
                animComponent->currentAnimation = handle;
                animComponent->isPlaying = true;
            }
        }

        // Add the component to the entity
        entity.addComponent<Animations::SkeletalAnimationComponent>(std::move(animComponent));
    }


#pragma endregion


#pragma region Helpers

    Vector2 EntityFactory::ReadVector2(const rapidjson::Value& vectorData) {
        return { vectorData[0].GetFloat(), vectorData[1].GetFloat() };
    }

#pragma endregion

} // namespace LaurelEye
