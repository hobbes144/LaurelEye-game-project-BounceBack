/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   11-20-2025
/// @brief Handles binding the C++ ui library to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/core/EngineContext.h"
#include <sol/sol.hpp>
#include <unordered_map>
#include <vector>
#include <string>

namespace LaurelEye::Scripting {

    class Sol2API_UI {
    public:
        static void setup(sol::state& lua, EngineContext* ctx);
        static void shutdown(EngineContext* ctx);
    private:
        static inline std::unordered_map<std::string, std::vector<sol::function>> buttonCallbacks;
        static inline uint32_t listenerID;
    };
} // namespace LaurelEye::Scripting
