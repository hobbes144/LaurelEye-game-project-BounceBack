#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptInstance.h"
#include "LaurelEyeEngine/scripting/IScriptInstance.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2API.h"
#include "LaurelEyeEngine/io/FileSystem.h"
#include "LaurelEyeEngine/physics/CollisionManager.h"
#include "LaurelEyeEngine/UI/UIComponents/UIInteractionComponent.h"
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

        //Collision Event Bindings
        collisionEnterFunc = env["onCollisionEnter"];
        collisionStayFunc = env["onCollisionStay"];
        collisionExitFunc = env["onCollisionExit"];

        //Trigger Event Bindings
        triggerEnterFunc = env["onTriggerEnter"];
        triggerStayFunc = env["onTriggerStay"];
        triggerExitFunc = env["onTriggerExit"];

        //UI Interaction Bindings
        hoverEnterFunc = env["onHoverEnter"];
        HoverExitFunc = env["onHoverExit"];
        pressedFunc = env["onPressed"];
        heldFunc = env["onHeld"];
        releasedFunc = env["onReleased"];
        clickedFunc = env["onClicked"];
        dragStartFunc = env["onDragStart"];
        draggingFunc = env["onDragging"];
        dragEndFunc = env["onDragEnd"];
        focusGainedFunc = env["onFocusGained"];
        focusLostFunc = env["onFocusLost"];

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

    //Trigger Functions
    void Sol2ScriptInstance::onTriggerEnter(const Physics::CollisionEventData& data) {
        if (triggerEnterFunc.valid()) {
            auto result = triggerEnterFunc(data);
            if (!result.valid()) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onTriggerEnter Error] " << err.what());
            }
        }
    }

    void Sol2ScriptInstance::onTriggerStay(const Physics::CollisionEventData& data) {
        if (triggerStayFunc.valid()) {
            auto result = triggerStayFunc(data);
            if (!result.valid()) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onTriggerStay Error] " << err.what());
            }
        }
    }

    void Sol2ScriptInstance::onTriggerExit(const Physics::CollisionEventData& data) {
        if (triggerExitFunc.valid()) {
            auto result = triggerExitFunc(data);
            if (!result.valid()) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onTriggerExit Error] " << err.what());
            }
        }
    }
    
    void Sol2ScriptInstance::onHoverEnter(const UI::UIInteractionEventData& data) {
        if ( hoverEnterFunc.valid() ) {
            auto result = hoverEnterFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onHoverEnter Error] " << err.what());
            }
        }
    }
    void Sol2ScriptInstance::onHoverExit(const UI::UIInteractionEventData& data) {
        if ( HoverExitFunc.valid() ) {
            auto result = HoverExitFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onHoverExit Error] " << err.what());
            }
        }
    }
    void Sol2ScriptInstance::onPressed(const UI::UIInteractionEventData& data) {
        if ( pressedFunc.valid() ) {
            auto result = pressedFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onPressed Error] " << err.what());
            }
        }
    }
    void Sol2ScriptInstance::onHeld(const UI::UIInteractionEventData& data) {
        if ( heldFunc.valid() ) {
            auto result = heldFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onHeld Error] " << err.what());
            }
        }
    }
    void Sol2ScriptInstance::onReleased(const UI::UIInteractionEventData& data) {
        if ( releasedFunc.valid() ) {
            auto result = releasedFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onReleased Error] " << err.what());
            }
        }
    }
    void Sol2ScriptInstance::onClicked(const UI::UIInteractionEventData& data) {
        if ( clickedFunc.valid() ) {
            auto result = clickedFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onClicked Error] " << err.what());
            }
        }
    }
    void Sol2ScriptInstance::onDragStart(const UI::UIInteractionEventData& data) {
        if ( dragStartFunc.valid() ) {
            auto result = dragStartFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onDragStart Error] " << err.what());
            }
        }
    }
    void Sol2ScriptInstance::onDragging(const UI::UIInteractionEventData& data) {
        if ( draggingFunc.valid() ) {
            auto result = draggingFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onDragging Error] " << err.what());
            }
        }
    }
    void Sol2ScriptInstance::onDragEnd(const UI::UIInteractionEventData& data) {
        if ( dragEndFunc.valid() ) {
            auto result = dragEndFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onDragEnd Error] " << err.what());
            }
        }
    }
    void Sol2ScriptInstance::onFocusGained(const UI::UIInteractionEventData& data) {
        if ( focusGainedFunc.valid() ) {
            auto result = focusGainedFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onFocusGained Error] " << err.what());
            }
        }
    }
    void Sol2ScriptInstance::onFocusLost(const UI::UIInteractionEventData& data) {
        if ( focusLostFunc.valid() ) {
            auto result = focusLostFunc(data);
            if ( !result.valid() ) {
                sol::error err = result;
                LE_DEBUG_ASSERT("Scripting", false, "[Lua onFocusLost Error] " << err.what());
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
        triggerEnterFunc = sol::nil;
        triggerStayFunc = sol::nil;
        triggerExitFunc = sol::nil;

        hoverEnterFunc = sol::nil;
        HoverExitFunc = sol::nil;
        pressedFunc = sol::nil;
        heldFunc = sol::nil;
        releasedFunc = sol::nil;
        clickedFunc = sol::nil;
        dragStartFunc = sol::nil;
        draggingFunc = sol::nil;
        dragEndFunc = sol::nil;
        focusGainedFunc = sol::nil;
        focusLostFunc = sol::nil;

        env = sol::nil;
        owner = nullptr;
    }
} // namespace LaurelEye::Scripting
