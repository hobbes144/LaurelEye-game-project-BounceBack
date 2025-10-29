/// @file   Sol2SciptInstance.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-7-2025
/// @brief Represents an actual Lua script to be used by Sol2 (Sol2 specific)
/// 
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include <sol/sol.hpp>
#include "LaurelEyeEngine/scripting/IScriptInstance.h"

namespace LaurelEye::Scripting{

    class Sol2ScriptInstance : public IScriptInstance {
    public:
        Sol2ScriptInstance(sol::state& luaState, const std::string& scriptPath, LaurelEye::Entity* owner_);

        void onStart() override;
        void onUpdate(float dt) override;
        void onShutdown() override;

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
        sol::function startFunc, updateFunc, shutdownFunc;
        LaurelEye::Entity* owner;

        void invalidate();
    };

} // namespace LaurelEye
