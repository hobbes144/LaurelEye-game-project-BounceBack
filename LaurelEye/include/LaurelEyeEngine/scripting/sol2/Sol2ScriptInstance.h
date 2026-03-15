/// @file   Sol2SciptInstance.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-7-2025
/// @brief Represents an actual Lua script to be used by Sol2 (Sol2 specific)
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once

#include "LaurelEyeEngine/scripting/IScriptInstance.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2Message.h"
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2ScriptInstance : public IScriptInstance {
    public:
        Sol2ScriptInstance(
            sol::state& luaState, const std::string& scriptPath,
            LaurelEye::Entity* owner_);
        ~Sol2ScriptInstance();
        void onStart() override;
        void onUpdate(float dt) override;
        void onShutdown() override;

        //Collision Event Functions
        void onCollisionEnter(const Physics::CollisionEventData& data) override;
        void onCollisionStay(const Physics::CollisionEventData& data) override;
        void onCollisionExit(const Physics::CollisionEventData& data) override;

        void onHoverEnter(const UI::UIInteractionEventData& data) override;
        void onHoverExit(const UI::UIInteractionEventData& data) override;
        void onPressed(const UI::UIInteractionEventData& data) override;
        void onHeld(const UI::UIInteractionEventData& data) override;
        void onReleased(const UI::UIInteractionEventData& data) override;
        void onClicked(const UI::UIInteractionEventData& data) override;
        void onDragStart(const UI::UIInteractionEventData& data) override;
        void onDragging(const UI::UIInteractionEventData& data) override;
        void onDragEnd(const UI::UIInteractionEventData& data) override;
        void onFocusGained(const UI::UIInteractionEventData& data) override;
        void onFocusLost(const UI::UIInteractionEventData& data) override;

        void onMessage(const SolMessage& message);

        //Trigger Event Functions
        void onTriggerEnter(const Physics::CollisionEventData& data) override;
        void onTriggerStay(const Physics::CollisionEventData& data) override;
        void onTriggerExit(const Physics::CollisionEventData& data) override;

        /// @brief Gets an active lua variable state from the sol2 environment.
        /// For example, if an int "count" is set from script, this lets us reach
        /// into the lua state and find that value.
        /// @tparam T The typing we are looking for from the variable name
        /// @param name Name of the variable
        /// @return the T typed variable if it exists
        template <typename T>
        std::optional<T> getVar(const std::string& name) const {
            sol::object obj = env[name];
            if ( obj == sol::lua_nil ) return std::nullopt;
            try {
                return obj.as<T>();
            }
            catch ( ... ) {
                return std::nullopt;
            }
        }

    private:
        sol::state& lua;
        sol::environment env;
        sol::function startFunc, updateFunc, shutdownFunc,
                      messageFunc,
                      collisionEnterFunc, collisionStayFunc, collisionExitFunc,
                      triggerEnterFunc, triggerStayFunc, triggerExitFunc;

        sol::function hoverEnterFunc, HoverExitFunc,
                      pressedFunc, heldFunc, releasedFunc, clickedFunc,
                      dragStartFunc, draggingFunc, dragEndFunc,
                      focusGainedFunc, focusLostFunc;
        LaurelEye::Entity* owner;

        void invalidate();
    };

} // namespace LaurelEye::Scripting
