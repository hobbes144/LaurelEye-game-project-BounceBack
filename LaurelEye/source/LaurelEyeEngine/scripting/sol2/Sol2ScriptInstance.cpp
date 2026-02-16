#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptInstance.h"
#include "LaurelEyeEngine/scripting/IScriptInstance.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2API.h"
#include "LaurelEyeEngine/io/FileSystem.h"
#include "LaurelEyeEngine/physics/CollisionManager.h"
#include "LaurelEyeEngine/ecs/Entity.h"
#include <cassert>
#include <sol/protected_function_result.hpp>

#include "LaurelEyeEngine/logging/EngineLog.h"

namespace LaurelEye::Scripting {

    Sol2ScriptInstance::Sol2ScriptInstance(
        sol::state& luaState, const std::string& scriptPath,
        LaurelEye::Entity* owner_)
        : lua(luaState),
          env(luaState, sol::create, luaState.globals()),
          owner(owner_) {

        Sol2API::registerEnvironment(env, owner);
        // TODO: somehow let AssetManager do this?
        auto fullPath = IO::resolve(scriptPath);

        // Load file and check if it loaded correctly.
#ifdef NDEBUG
        const sol::protected_function_result scriptLoadResult =
            lua.script_file(fullPath.string(), env);
#else
        const sol::protected_function_result scriptLoadResult =
            lua.safe_script_file(fullPath.string(), env);
#endif
        LE_ASSERT("Scripting", scriptLoadResult.valid(), "Failed to load file: " << scriptPath);

        startFunc = env["onStart"];
        updateFunc = env["onUpdate"];
        shutdownFunc = env["onShutdown"];

        messageFunc = env["onMessage"];

        collisionEnterFunc = env["onCollisionEnter"];
        collisionStayFunc = env["onCollisionStay"];
        collisionExitFunc = env["onCollisionExit"];
    }

    Sol2ScriptInstance::~Sol2ScriptInstance() {
        invalidate();
    }

    void Sol2ScriptInstance::onStart() {
        if ( startFunc.valid() ) {
            auto result = startFunc();
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onStart Error] " << err.what());
            }
        }
    }

    void Sol2ScriptInstance::onUpdate(float dt) {
        if ( updateFunc.valid() ) {
            auto result = updateFunc(dt);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onUpdate Error] " << err.what());
            }
        }
    }

    void Sol2ScriptInstance::onShutdown() {
        if ( shutdownFunc.valid() ) {
            auto result = shutdownFunc();
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onShutdown Error] " << err.what());
            }
        }
        invalidate();
    }

    void Sol2ScriptInstance::onMessage(const SolMessage& msg) {
        if ( messageFunc.valid() ) {
            auto result = messageFunc(msg);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onMessage Error] " << err.what());
            }
        }
    }

    void Sol2ScriptInstance::onCollisionEnter(const Physics::CollisionEventData& data) {
        if ( collisionEnterFunc.valid() ) {
            auto result = collisionEnterFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onCollisionEnter Error] " << err.what());
            }
        }
    }

    void Sol2ScriptInstance::onCollisionStay(const Physics::CollisionEventData& data) {
        if ( collisionStayFunc.valid() ) {
            auto result = collisionStayFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onCollisionStay Error] " << err.what());
            }
        }
    }

    void Sol2ScriptInstance::onCollisionExit(const Physics::CollisionEventData& data) {
        if ( collisionExitFunc.valid() ) {
            auto result = collisionExitFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onCollisionExit Error] " << err.what());
            }
        }
    }

    void Sol2ScriptInstance::invalidate() {
        startFunc = sol::nil;
        updateFunc = sol::nil;
        shutdownFunc = sol::nil;
        collisionEnterFunc = sol::nil;
        collisionStayFunc = sol::nil;
        collisionExitFunc = sol::nil;
        env = sol::nil;
        owner = nullptr;
    }
} // namespace LaurelEye::Scripting
