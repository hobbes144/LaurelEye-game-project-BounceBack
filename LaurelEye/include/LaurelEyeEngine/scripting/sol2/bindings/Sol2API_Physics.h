/// @author Jacob Wilburn (jacoblinwilburn@gmail.com)
/// @par    **DigiPen Email**
///     jacob.wilburn@digipen.edu
/// @date   11-5-2025
/// @brief Handles binding the C++ physics library to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/core/EngineContext.h"
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API_Physics {
    public:
        /// @brief Called by Sol2API in order to register all input bindings
        /// @param lua Lua state
        static void setup(sol::state& lua, EngineContext* ctx);

    private:

    };

} // namespace LaurelEye::Scripting
