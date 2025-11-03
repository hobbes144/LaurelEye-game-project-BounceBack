#include "LaurelEyeEngine/ecs/EntityFactory.h"
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/scene/Scene.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/Renderable3DComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/CameraComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/DirectionalLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/PointLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/AmbientLightComponent.h"
#include "LaurelEyeEngine/particles/ParticleEmitterComponent.h"
#include "LaurelEyeEngine/physics/PhysicsBodyComponent.h"
#include "LaurelEyeEngine/physics/interfaces/PhysicsTypes.h"
#include "LaurelEyeEngine/scripting/ScriptComponent.h"
#include "LaurelEyeEngine/io/AssetManager.h"
#include "LaurelEyeEngine/graphics/RenderSystem.h"
#include <iostream>

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

    std::unique_ptr<Entity> EntityFactory::createEntityFromJson(const rapidjson::Value& entityJson) {
        if ( !entityJson.HasMember("name") || !entityJson["name"].IsString() )
            throw std::runtime_error("EntityFactory: Entity missing 'name' field");

        std::string name = entityJson["name"].GetString();
        auto entity = std::make_unique<Entity>(name);

        // Add components if present
        if ( entityJson.HasMember("components") && entityJson["components"].IsObject() ) {
            const auto& comps = entityJson["components"];
            addComponentsFromJson(*entity, comps);
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
            else {
                std::cerr << "Entity Factory: Unknown component type trying to be added to entity: " << entity.getName() << std::endl;
            }
        }
    }

    void EntityFactory::createAndAddEntitiesToScene(Scene& scene,
                                                    const rapidjson::GenericArray<true, rapidjson::Value>& entities,
                                                    std::unordered_map<std::string, Entity*>& entityMap) {
        for ( const auto& entityData : entities ) {
            auto entity = createEntityFromJson(entityData);
            Entity* rawPtr = entity.get();
            entityMap[rawPtr->getName()] = rawPtr;
            scene.addEntity(std::move(entity));
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
                        parentTransform->addChild(childTransform);
                    }
                }
            }
        }
    }

/// This region defines component setups for the entity
/// Ideally this is done by the systems I think, but this is okay for now.
    #pragma region Setup

    void EntityFactory::setupTransformComponent(Entity& entity, const rapidjson::Value& transformData) {
        TransformComponent transform;
        if ( transformData.HasMember("position") ) {
            const auto& pos = transformData["position"];
            transform.setLocalPosition({pos[0].GetFloat(), pos[1].GetFloat(), pos[2].GetFloat()});
        }
        if ( transformData.HasMember("rotation") ) { // Quaternion
            const auto& rot = transformData["rotation"];
            transform.setLocalRotation({rot[0].GetFloat(), rot[1].GetFloat(), rot[2].GetFloat(), rot[3].GetFloat()});
        }
        if ( transformData.HasMember("scale") ) {
            const auto& scale = transformData["scale"];
            transform.setLocalScale({scale[0].GetFloat(), scale[1].GetFloat(), scale[2].GetFloat()});
        }
        entity.addComponent<TransformComponent>(transform);
    }

    void EntityFactory::setupRender3DComponent(Entity& entity, const rapidjson::Value& render3DData) {
        // Register renderer component

        //Make the Renderable Component
        std::unique_ptr<Graphics::Renderable3DComponent> renderComponent = std::make_unique<Graphics::Renderable3DComponent>();
       //---Mesh---
        //If a Mesh member is identified as a string
        if (render3DData.HasMember("mesh") && render3DData["mesh"].IsObject()) {
            const auto& mesh = render3DData["mesh"];
            if ( mesh.HasMember("file") && mesh["file"].IsString() ) {
                std::string meshPath = mesh["file"].GetString();
                auto asset = context.getService<IO::AssetManager>()->load(assetPath + meshPath);
                auto meshAsset = std::dynamic_pointer_cast<IO::MeshAsset>(asset);
                if ( !meshAsset ) {
                    throw((void)("EntityFactory::setupRender3DComponent - Asset '{}' is not a MeshAsset."), meshPath);
                    return;
                }
                auto meshObj = Graphics::Mesh::createMeshFromAsset(meshAsset);
                renderComponent->SetMesh(meshObj);
            }

            else if (mesh.HasMember("primitiveType") && mesh["primitiveType"].IsString()) {
                std::string shapeType = mesh["primitiveType"].GetString();
                if ( shapeType == "Square" ) {
                    renderComponent->SetMesh(Graphics::Mesh::getShapeMesh(Graphics::Mesh::Square));
                }
                else if ( shapeType == "Cube" ) {
                    renderComponent->SetMesh(Graphics::Mesh::getShapeMesh(Graphics::Mesh::Cube));
                }
                else if (shapeType == "Sphere") {
                    renderComponent->SetMesh(Graphics::Mesh::getShapeMesh(Graphics::Mesh::Sphere));
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
                auto handle = context.getService<Graphics::RenderSystem>()->getRenderResources()->texture(texAsset->getName());

                renderComponent->GetMaterial()->setTexture("mainTexture", handle);
                renderComponent->GetMaterial()->setProperty<int>("useTexture", 1);
            }
            else {
                renderComponent->GetMaterial()->setProperty<int>("useTexture", 0);
            }

            if ( material.HasMember("mainTextureScale") && material["mainTextureScale"].IsArray() ) {
                const auto& scale = material["mainTextureScale"].GetArray();
                renderComponent->GetMaterial()->setProperty<Vector2>(
                    "mainTextureScale",
                    Vector2({scale[0].GetFloat(), scale[1].GetFloat()}));
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

        cameraComponent->setPositionRotation(
            Vector3(-50.f, 15.f, 0.f),
            Matrix4::lookAt(Vector3(-50.f, 15.f, 0.f), Vector3(0.f)).toQuaternion());
        cameraComponent->setPerspectiveProjection(45.0f * 3.14159f / 180.0f, 1280.f / 720.f, 0.1f, 1000.0f);
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

        if ( physicsData.HasMember("linearDamping") ) data.linearDamping = physicsData["linearDamping"].GetFloat();
        if ( physicsData.HasMember("angularDamping") ) data.angularDamping = physicsData["angularDamping"].GetFloat();
        if ( physicsData.HasMember("friction") ) data.friction = physicsData["friction"].GetFloat();
        if ( physicsData.HasMember("restitution") ) data.restitution = physicsData["restitution"].GetFloat();

        if ( physicsData.HasMember("collisionGroup") ) data.collisionGroup = physicsData["collisionGroup"].GetUint();
        if ( physicsData.HasMember("collisionMask") ) data.collisionMask = physicsData["collisionMask"].GetUint();

        if ( physicsData.HasMember("useCCD") ) data.useCCD = physicsData["useCCD"].GetBool();
        if ( physicsData.HasMember("ccdMotionThreshold") ) data.ccdMotionThreshold = physicsData["ccdMotionThreshold"].GetFloat();
        if ( physicsData.HasMember("ccdSweptSphereRadius") ) data.ccdSweptSphereRadius = physicsData["ccdSweptSphereRadius"].GetFloat();

        if ( physicsData.HasMember("gravityScale") ) data.gravityScale = physicsData["gravityScale"].GetFloat();

        // --- Assign to component ---
        auto physicsComp = entity.addComponent<PhysicsBodyComponent>(data);
    }

    void EntityFactory::setupScriptComponent(Entity& entity, const rapidjson::Value& scriptData) {
        // Register script component
        if ( !scriptData.HasMember("path") || !scriptData["path"].IsString() ) {
            std::cerr << "EntityFactory: Script component missing 'path' for entity: "
                      << entity.getName() << std::endl;
            return;
        }

        std::string relPath = scriptData["path"].GetString();
        std::string resolvedPath = assetPath + relPath;

        entity.addComponent<Scripting::ScriptComponent>(resolvedPath);
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

    #pragma endregion


} // namespace LaurelEye
