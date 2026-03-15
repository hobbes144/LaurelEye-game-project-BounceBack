/// @author Jacob Wilburn (jacoblinwilburn@gmail.com)
/// @par    **DigiPen Email**
///     jacob.wilburn@digipen.edu
/// @date   3-14-2026
/// @brief Handles binding the C++ game manager library to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
/// 
#pragma once

#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/gameManager/GameManager.h"
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API_GameManager {
    public:
        static void setup(sol::state& lua, EngineContext* ctx);

    private:
        static void setupGameManager(sol::state& lua, GameManager* manager);
    };

} // namespace LaurelEye::Scripting
