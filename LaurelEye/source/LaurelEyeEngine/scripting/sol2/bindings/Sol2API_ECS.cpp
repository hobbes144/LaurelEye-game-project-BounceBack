#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_ECS.h"
#include "LaurelEyeEngine/core/Engine.h"
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/Renderable3DComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/Renderable2DComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/CameraComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/DirectionalLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/AmbientLightComponent.h"
#include "LaurelEyeEngine/graphics/graphics_components/PointLightComponent.h"
#include "LaurelEyeEngine/physics/PhysicsBodyBaseComponent.h"
#include "LaurelEyeEngine/physics/RigidBodyComponent.h"
#include "LaurelEyeEngine/physics/GhostBodyComponent.h"
#include "LaurelEyeEngine/scripting/ScriptComponent.h"
#include "LaurelEyeEngine/audio/SpeakerComponent.h"
#include "LaurelEyeEngine/particles/ParticleEmitterComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UITransformComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UIRenderComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UIInteractionComponent.h"
#include "LaurelEyeEngine/audio/SpeakerComponent.h"
#include "LaurelEyeEngine/ecs/IComponent.h"
#include "LaurelEyeEngine/animation/components/SkeletalAnimationComponent.h"
#include "LaurelEyeEngine/logging/EngineLog.h"
#include "LaurelEyeEngine/audio/AudioComponent.h"
#include "LaurelEyeEngine/UI/UIComponents/UITextComponent.h"

namespace LaurelEye::Scripting {
    void Sol2API_ECS::setup(sol::state& lua, EngineContext* ctx) {
        setupEntityType(lua);

        // Engine setup - TODO move to engine binding file later
        auto* engine = ctx->getService<Engine>();
        LE_ASSERT("scripting", engine, "Engine service not found in context!");

        auto timeDataType = lua.new_usertype<TimeData>("TimeData");
        timeDataType["scaledTime"] = &TimeData::scaledTime;
        timeDataType["unscaledTime"] = &TimeData::unscaledTime;
        timeDataType["scaledDt"] = &TimeData::scaledDt;
        timeDataType["unscaledDt"] = &TimeData::unscaledDt;

        sol::table Engine = lua["Engine"].get_or_create<sol::table>();
        Engine.set_function("stop",
                             [engine]() -> void {
                                 engine->stop();
                             });
        Engine.set_function("setTimeScale",
                             [engine](float timeScale) -> void {
                                 engine->setTimeScale(timeScale);
                             });
        Engine.set_function("getTimeScale",
                             [engine]() -> float {
                                 return engine->getTimeScale();
                             });
        Engine.set_function("time",
                             [ctx]() -> TimeData {
                                 return ctx->time();
                             });
    }

    void Sol2API_ECS::setupEntityType(sol::state& lua) {
        auto entityType = lua.new_usertype<Entity>(
            "Entity",
            sol::no_constructor,
            "getId", &Entity::getId,
            "getName", &Entity::getName,
            "addTag", &Entity::addTag,
            "compareTag", &Entity::compareTag,
            "getTags", &Entity::getTags,
            "getPlayerHealth", &Entity::getPlayerHealth,
            "setPlayerHealth", &Entity::setPlayerHealth,
            "findComponent", &dynamicFindComponent,
            "setActive",
            [](Entity& ent, bool active) {
                const auto& comps = ent.getComponents();
                for ( const auto& comp : comps ) {
                    if ( comp ) {
                        comp->setIsActive(active);
                    }
                }
            });

        // --- Type-safe finders (fast path) ---
        entityType.set_function("findTransform", &safeFindComponent<TransformComponent>);
        entityType.set_function("findRender3D", &safeFindComponent<Graphics::Renderable3DComponent>);
        entityType.set_function("findRender2D", &safeFindComponent<Graphics::Renderable2DComponent>);
        entityType.set_function("findDirectionalLight", &safeFindComponent<Graphics::DirectionalLightComponent>);
        entityType.set_function("findAmbientLight", &safeFindComponent<Graphics::AmbientLightComponent>);
        entityType.set_function("findPointLight", &safeFindComponent<Graphics::PointLightComponent>);
        entityType.set_function("findRigidBody", &safeFindComponent<Physics::RigidBodyComponent>);
        entityType.set_function("findGhostBody", &safeFindComponent<Physics::GhostBodyComponent>);
        //entityType.set_function("findPhysics", &safeFindComponent<Physics::PhysicsBodyBaseComponent>);
        entityType.set_function("findScript", &safeFindComponent<ScriptComponent>);
        entityType.set_function("findParticleEmitter", &safeFindComponent<Particles::ParticleEmitterComponent>);
        entityType.set_function("findAudio", &safeFindComponent<Audio::AudioComponent>);
        entityType.set_function("findUIRender", &safeFindComponent<UI::UIRenderComponent>);
        entityType.set_function("findUIText", &safeFindComponent<UI::UITextComponent>);
        entityType.set_function("findUITransform", &safeFindComponent<UI::UITransformComponent>);
        entityType.set_function("findUIInteraction", &safeFindComponent<UI::UIInteractionComponent>);
        entityType.set_function("findPointLight", &safeFindComponent<Graphics::PointLightComponent>);
        entityType.set_function("findAnimator", &safeFindComponent<Animations::SkeletalAnimationComponent>);
    }

    sol::object Sol2API_ECS::dynamicFindComponent(Entity& e, const std::string& type, sol::this_state s) {
        sol::state_view lua(s);

        if ( type == "TransformComponent" ) {
            if ( auto* c = e.findComponent<TransformComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "Renderable3DComponent" ) {
            if ( auto* c = e.findComponent<Graphics::Renderable3DComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "Renderable2DComponent" ) {
            if ( auto* c = e.findComponent<Graphics::Renderable2DComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "DirectionalLightComponent" ) {
            if ( auto* c = e.findComponent<Graphics::DirectionalLightComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "AmbientLightComponent" ) {
            if ( auto* c = e.findComponent<Graphics::AmbientLightComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "PointLightComponent" ) {
            if ( auto* c = e.findComponent<Graphics::PointLightComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "PhysicsBodyComponent" ) {
            if ( auto* c = e.findComponent<Physics::PhysicsBodyBaseComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "ParticleEmitterComponent" ) {
            if ( auto* c = e.findComponent<Particles::ParticleEmitterComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "ScriptComponent" ) {
            if ( auto* c = e.findComponent<ScriptComponent>() )
                return sol::make_object(lua, c);
        }
        //else if ( type == "SpeakerComponent" ) {
        //    if ( auto* c = e.findComponent<Audio::SpeakerComponent>() )
        //        return sol::make_object(lua, c);
        //}
        else if ( type == "AudioComponent" ) {
            if ( auto* c = e.findComponent<Audio::AudioComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "PointLightComponent" ) {
            if ( auto* c = e.findComponent<Graphics::PointLightComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "SkeletalAnimationComponent" ) {
            if ( auto* c = e.findComponent<Animations::SkeletalAnimationComponent>() )
            return sol::make_object(lua, c);
        }
        else if ( type == "CameraComponent" ) {
            if ( auto* c = e.findComponent<Graphics::CameraComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "UITransformComponent" ) {
            if ( auto* c = e.findComponent<UI::UITransformComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "UIRenderComponent" ) {
            if ( auto* c = e.findComponent<UI::UIRenderComponent>() )
                return sol::make_object(lua, c);
        }
        else if ( type == "UITextComponent" ) {
            if ( auto* c = e.findComponent<UI::UITextComponent>() )
                return sol::make_object(lua, c);
        }

        return sol::make_object(lua, sol::nil);
    }


} // namespace LaurelEye::Scripting
