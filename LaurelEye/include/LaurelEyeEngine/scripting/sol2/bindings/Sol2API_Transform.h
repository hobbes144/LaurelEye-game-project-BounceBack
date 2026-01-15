/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-29-2025
/// @brief Handles binding the C++ transform system to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API_Transform {
    public:
        /// @brief Called by Sol2API in order to register all transform bindings
        /// @param lua Lua state
        static void setup(sol::state& lua);
    };

} // namespace LaurelEye::Scripting
