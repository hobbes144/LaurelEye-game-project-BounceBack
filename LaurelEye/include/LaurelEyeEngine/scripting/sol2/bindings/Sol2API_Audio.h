/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   11-19-2025
/// @brief Handles binding the C++ audio library to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/core/EngineContext.h"
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API_Audio {
    public:
        /// @brief Called by Sol2API in order to register all audio bindings
        /// @param lua Lua state
        static void setup(sol::state& lua, EngineContext* ctx);

    private:
    };

} // namespace LaurelEye::Scripting
