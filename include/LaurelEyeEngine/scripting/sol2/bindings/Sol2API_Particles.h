///
/// @file   Sol2API_Particles.h
/// @author Nicholas Shaw (nick_shaw@me.com)
/// @par    **DigiPen Email**
///    nick.shaw@digipen.edu
/// @par    **Course**
///    GAM541
/// @date   11-17-2025
/// @brief  Particle Integration
///

#pragma once
#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/particles/ParticleEmitterComponent.h"
#include <sol/sol.hpp>

namespace LaurelEye::Scripting {

    class Sol2API_Particles {
    public:
        /// @brief Called by Sol2API in order to register all input bindings
        /// @param lua Lua state
        static void setup(sol::state& lua, EngineContext* ctx);

    private:
    };

} // namespace LaurelEye::Scripting
