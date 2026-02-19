/// @file   Sol2API_Window.h
/// @author Anish Murthy (anish.murthy.dev@gmail.com)
/// @par    **DigiPen Email**
///     anish.murthy@digipen.edu
/// @date    02-16-2026
/// @brief Scripting for Window control actions

#pragma once
#include <sol/sol.hpp>

namespace LaurelEye {
    class EngineContext;
}

namespace LaurelEye::Scripting {

    class Sol2API_Window {
    public:
        /// @brief Called by Sol2API in order to register all math bindings
        /// @param lua Lua state
        static void setup(sol::state& lua, EngineContext* ctx);
    };

} // namespace LaurelEye::Scripting
