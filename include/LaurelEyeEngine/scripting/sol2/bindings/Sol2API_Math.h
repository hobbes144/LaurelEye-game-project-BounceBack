/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-16-2025
/// @brief Handles binding the C++ math library to lua.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/math/Matrix4.h"
#include "LaurelEyeEngine/math/Transform.h"
#include <cmath>
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API_Math {
    public:
        /// @brief Called by Sol2API in order to register all math bindings
        /// @param lua Lua state
        static void setup(sol::state& lua);

    private:
        // Setting up individual math structures
        static void setupVec3(sol::state& lua);
        static void setupMatrix4(sol::state& lua);
        static void setupQuaternion(sol::state& lua);
        static void setupTransform(sol::state& lua);
    };

} // namespace LaurelEye::Scripting
