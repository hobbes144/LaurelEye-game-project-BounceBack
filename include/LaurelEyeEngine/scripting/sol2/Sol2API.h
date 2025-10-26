/// @file   Sol2API.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-16-2025
/// @brief Registers the engine's internal systems with the living sol2 state,
/// allowing all registered scripts to have access to them. For example, logging,
/// coroutines, input, math, etc can all be accessed from Lua
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/ecs/Entity.h"
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API {
    public:
        /// @brief Registers api state wide (all scripts can access this). This is for systems and core logics
        /// @param lua The lua state to register - has controll of all scripts attached to it
        static void registerState(sol::state& lua);

        /// @brief Registers api state per environment (lua script instance). This would be per entity variables (mainly just entity itself)
        /// @param env The script env instance
        /// @param owner The entity owner of the script
        static void registerEnvironment(sol::environment& env, LaurelEye::Entity* owner);

    private:
        // Logging helpers
        static void log(const std::string& msg);
        static void logErr(const std::string& msg);
        static void logWarn(const std::string& msg);
    };

} // namespace LaurelEye::Scripting
