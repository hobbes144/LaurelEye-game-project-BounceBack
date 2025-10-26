/// @file   Sol2State.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   10-7-2025
/// @brief  A manager of scripting state/world for Sol2, a framwork for Lua/CPP integration
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include <memory>
#include <string>
#include <sol/sol.hpp>
#include "LaurelEyeEngine/scripting/IScriptEngineState.h"
#include "LaurelEyeEngine/scripting/IScriptInstance.h"

namespace LaurelEye::Scripting {
    class Sol2State : public IScriptEngineState{
    public:
        void initialize() override;
        void shutdown() override;
        std::unique_ptr<IScriptInstance> createInstance(const std::string& path, LaurelEye::Entity* owner) override;

        const sol::state& getState() const { return lua; }
    private:
        sol::state lua;
        friend class LuaScriptInstance;
    };
} // namespace LaurelEye::Scripting
