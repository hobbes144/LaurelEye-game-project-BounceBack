/// @author Martin Chow
/// @par    **DigiPen Email**
///     martin.chow@digipen.edu
/// @date   11-20-2025
/// @brief Handles binding the C++ ui library to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/core/EngineContext.h"
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API_UIText {
    public:
        static void setup(sol::state& lua, EngineContext* ctx);
        static void shutdown(EngineContext* ctx);
    private:
        static void setupUITextComponent(sol::state& lua);

    };
} // namespace LaurelEye::Scripting
