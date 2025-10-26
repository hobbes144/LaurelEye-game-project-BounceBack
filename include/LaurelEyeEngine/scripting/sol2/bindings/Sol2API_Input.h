/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-16-2025
/// @brief Handles binding the C++ input library to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/input/InputManager.h"
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API_Input {
    public:
        /// @brief Called by Sol2API in order to register all input bindings
        /// @param lua Lua state
        static void setup(sol::state& lua);

    private:
        static void setupInputManager(sol::state& lua);
    };

} // namespace LaurelEye::Scripting
