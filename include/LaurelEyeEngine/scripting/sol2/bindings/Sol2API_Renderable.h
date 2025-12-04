/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   11-12-2025
/// @brief Handles binding the C++ renderable library to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/core/EngineContext.h"
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API_Renderable{
    public:
        /// @brief Called by Sol2API in order to register all renderable bindings
        /// @param lua Lua state
        static void setup(sol::state& lua, EngineContext* ctx);

    private:
        static void setupMesh(sol::state& lua, EngineContext* ctx);
        static void setupMaterial(sol::state& lua, EngineContext* ctx);
        static void setupIRenderable(sol::state& lua);
        static void setupRenderer3D(sol::state& lua);
        static void setupRenderer2D(sol::state& lua);
    };

} // namespace LaurelEye::Scripting
