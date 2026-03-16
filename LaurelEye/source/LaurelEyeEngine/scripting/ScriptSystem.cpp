#include "LaurelEyeEngine/scripting/ScriptSystem.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptBroker.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2State.h"
#include "LaurelEyeEngine/scripting/ScriptTypeRegistry.h"
#include "LaurelEyeEngine/scene/SceneManager.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"

#include "LaurelEyeEngine/logging/EngineLog.h"

#include <optional>


namespace LaurelEye::Scripting {
    ScriptSystem::ScriptSystem(ScriptSystemType type) {
        systemType = type;
    }

    void ScriptSystem::initialize() {
        switch ( systemType ) {
        case ScriptSystemType::Sol2:
            scriptEngineState = std::make_unique<Sol2State>();
            broker = std::make_unique<Sol2ScriptBroker>();
            break;
        }
        scriptEngineState->initialize(context);

        registerScriptTypes();
    }

    void ScriptSystem::update(float deltaTime) {

        // Make a snapshot of the component pointers so we can iterate safely
        // even if registerComponent() / deregisterComponent() mutates `components`
        // a better bandaid fix, but could become a pain point for 1000's of scripts
        std::vector<ComponentPtr> snapshot;
        snapshot.reserve(components.size());
        snapshot.insert(snapshot.end(), components.begin(), components.end());

        for ( auto* comp : snapshot ) {
            if ( comp->isActive() && comp->getScriptInstance() ) {
                comp->getScriptInstance()->onUpdate(deltaTime);
            }
        }
        broker->flushMessageQueue();
    }

    void ScriptSystem::shutdown() {
        broker->clearMessageQueue();
        for ( auto* comp : components ) {
            if ( comp->getScriptInstance() ) {
                comp->getScriptInstance()->onShutdown();
                comp->destroyScriptInstance();
            }
        }
        if ( scriptEngineState ) {
            scriptEngineState->shutdown(context);
            scriptEngineState.reset();
        }
    }

    void ScriptSystem::registerComponent(const ComponentPtr comp) {
        if ( !comp->getScriptPath().empty() ) {
            comp->setScriptInstance(scriptEngineState->createInstance(comp->getScriptPath(), comp->getOwner()));
            if ( comp->getScriptInstance() ) {
                broker->registerScript(comp->getOwner(), comp->getScriptInstance());
                comp->getScriptInstance()->onStart();
            }
        }
        ISystem<ScriptComponent>::registerComponent(comp);
    }

    void ScriptSystem::deregisterComponent(const ComponentPtr comp) {
        if ( !comp ) return;
        // Clean up Lua instance
        if ( auto* inst = comp->getScriptInstance() ) {
            broker->deregisterScript(comp->getOwner());
            inst->onShutdown(); // Let the Lua script handle cleanup
            comp->destroyScriptInstance();
        }
        ISystem<ScriptComponent>::deregisterComponent(comp);
    }

    void ScriptSystem::registerScriptTypes() {

        //---------Primitives---------

        //Bool
        ScriptTypeRegistry::registerParser(
            "Bool",
            [](const rapidjson::Value& v) -> std::optional<ScriptValue> {
                if ( !v.IsBool() ) {
                    LE_DEBUG_WARN("ScriptSystem::ScriptTypeRegistry", "Type Value Mismatch | Bool <- " << v.GetType());
                    return std::nullopt;
                }
                ScriptValue sv = ScriptValue(v.GetBool());
                return sv;
            });

        //Int
        ScriptTypeRegistry::registerParser(
            "Int",
            [](const rapidjson::Value& v) -> std::optional<ScriptValue> {
                if ( !v.IsInt() ) {
                    LE_DEBUG_WARN("ScriptSystem::ScriptTypeRegistry", "Type Value Mismatch | Int <- " << v.GetType());
                    return std::nullopt;
                }
                ScriptValue sv = ScriptValue(v.GetInt());
                return sv;
            });

        //Float
        ScriptTypeRegistry::registerParser(
            "Float",
            [](const rapidjson::Value& v) -> std::optional<ScriptValue> {
                if ( !v.IsNumber() ) {
                    LE_DEBUG_WARN("ScriptSystem::ScriptTypeRegistry", "Type Value Mismatch | Float <- " << v.GetType());
                    return std::nullopt;
                }
                return ScriptValue(v.GetFloat());
            });

        // Double
        ScriptTypeRegistry::registerParser(
            "Double",
            [](const rapidjson::Value& v) -> std::optional<ScriptValue> {
                if ( !v.IsNumber() ) {
                    LE_DEBUG_WARN("ScriptSystem::ScriptTypeRegistry", "Type Value Mismatch | Double <- " << v.GetType());
                    return std::nullopt;
                }
                return ScriptValue(v.GetFloat());
            });

        //String
        ScriptTypeRegistry::registerParser(
            "String",
            [](const rapidjson::Value& v) -> std::optional<ScriptValue> {
                if ( !v.IsString() ) {
                    LE_DEBUG_WARN("ScriptSystem::ScriptTypeRegistry", "Type Value Mismatch | String <- " << v.GetType());
                    return std::nullopt;
                }
                return ScriptValue(std::string(v.GetString()));
            });

        //---------Vectors---------
        //Vector2
        ScriptTypeRegistry::registerParser(
            "Vector2",
            [](const rapidjson::Value& v) -> std::optional<ScriptValue> {
                if ( !v.IsObject() ||
                     !v.HasMember("x") || !v.HasMember("y") || 
                     !v["x"].IsNumber() || !v["y"].IsNumber() ||
                     v.MemberCount() != 2 ) {
                    LE_DEBUG_WARN("ScriptSystem::ScriptTypeRegistry", "Invalid Vector2 | Type: " << v.GetType());
                    return std::nullopt;
                }

                Vector2 vec2;

                vec2[0] = v["x"].GetFloat();
                vec2[1] = v["y"].GetFloat();

                return ScriptValue(vec2);
            });

        //Vector3
        ScriptTypeRegistry::registerParser(
            "Vector3",
            [](const rapidjson::Value& v) -> std::optional<ScriptValue> {
                if ( !v.IsObject() ||
                     !v.HasMember("x") || !v.HasMember("y") || !v.HasMember("z") ||
                     !v["x"].IsNumber() || !v["y"].IsNumber() || !v["z"].IsNumber() ||
                     v.MemberCount() != 3 ) {
                    LE_DEBUG_WARN("ScriptSystem::ScriptTypeRegistry", "Invalid Vector3 | Type: " << v.GetType());
                    return std::nullopt;
                }

                Vector3 vec3;

                vec3.x = v["x"].GetFloat();
                vec3.y = v["y"].GetFloat();
                vec3.z = v["z"].GetFloat();

                return ScriptValue(vec3);
            });

        //Vector4
        ScriptTypeRegistry::registerParser(
            "Vector4",
            [](const rapidjson::Value& v) -> std::optional<ScriptValue> {
                if ( !v.IsObject() || v.MemberCount() != 4 ) {
                    LE_DEBUG_WARN("ScriptSystem::ScriptTypeRegistry", "Invalid Vector4 | Type: " << v.GetType());
                    return std::nullopt;
                }

                Vector4 vec4;

                if ( (v.HasMember("x") && v.HasMember("y") && v.HasMember("z") && v.HasMember("w") &&
                      v["x"].IsNumber() && v["y"].IsNumber() && v["z"].IsNumber() && v["w"].IsNumber()) ) {
                    vec4[0] = v["x"].GetFloat();
                    vec4[1] = v["y"].GetFloat();
                    vec4[2] = v["z"].GetFloat();
                    vec4[3] = v["w"].GetFloat();

                    return ScriptValue(vec4);
                }
                else if ( (v.HasMember("r") && v.HasMember("g") && v.HasMember("b") && v.HasMember("a") &&
                           v["r"].IsNumber() && v["g"].IsNumber() && v["b"].IsNumber() && v["a"].IsNumber()) ) {
                    vec4[0] = v["r"].GetFloat();
                    vec4[1] = v["g"].GetFloat();
                    vec4[2] = v["b"].GetFloat();
                    vec4[3] = v["a"].GetFloat();

                    return ScriptValue(vec4);
                }

                LE_DEBUG_WARN("ScriptSystem::ScriptTypeRegistry", "Invalid Vector4 | Type: " << v.GetType());
                return std::nullopt;
                
            });

        //---------References---------
        //Complex Object
        ScriptTypeRegistry::registerParser(
            "Object",
            [](const rapidjson::Value& v) -> std::optional<ScriptValue> {
                if ( !v.IsObject() ) {
                    return std::nullopt;
                }

                return ScriptTypeRegistry::ParseDynamicValue(v);
            });

        //Array
        ScriptTypeRegistry::registerParser(
            "Array",
            [](const rapidjson::Value& v) -> std::optional<ScriptValue> {
                if ( !v.IsArray() ) {
                    return std::nullopt;
                }

                return ScriptTypeRegistry::ParseDynamicValue(v);
            });
        
        //Entity
        ScriptTypeRegistry::registerParser(
            "Entity",
            [this](const rapidjson::Value& v) -> std::optional<ScriptValue> {
                if ( !v.IsString()) {
                    return std::nullopt;
                }

                std::string name = v.GetString();

                if ( name.empty() || name[0] != '@' ) {
                    return std::nullopt;
                }

                std::string entityName = name.substr(1);

                Entity* ref =
                    context->getService<SceneManager>()
                        ->getCurrentScene()
                        ->findEntityByName(entityName);

                if ( !ref ) {
                    return std::nullopt;
                }

                return ScriptValue(ref);
            });

        //Transform
        ScriptTypeRegistry::registerParser(
            "Transform",
            [this](const rapidjson::Value& v) -> std::optional<ScriptValue> {
                if ( !v.IsString() ) {
                    return std::nullopt;
                }

                std::string name = v.GetString();

                if ( name.empty() || name[0] != '@' ) {
                    return std::nullopt;
                }

                std::string entityName = name.substr(1);

                Entity* ref =
                    context->getService<SceneManager>()
                        ->getCurrentScene()
                        ->findEntityByName(entityName);

                if ( !ref ) {
                    return std::nullopt;
                }
                
                TransformComponent* transComp = ref->findComponent<TransformComponent>();

                if ( !transComp ) {
                    return std::nullopt;
                }

                return ScriptValue(transComp);
            });

    }

} // namespace LaurelEye::Scripting
