/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   11-12-2025
/// @brief Handles binding the C++ lights library to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API_Lights {
    public:
        /// @brief Called by Sol2API in order to register all light bindings
        /// @param lua Lua state
        static void setup(sol::state& lua);
    };

} // namespace LaurelEye::Scripting
