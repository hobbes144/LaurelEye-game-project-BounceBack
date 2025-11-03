/// @file   ScriptSystem.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-24-2025
/// @brief  A system to handle the management and interface between external
/// scripting files and internal systems
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/ecs/ISystem.h"
#include "LaurelEyeEngine/scripting/ScriptComponent.h"
#include "LaurelEyeEngine/scripting/IScriptEngineState.h"
#include <memory>

namespace LaurelEye::Scripting {
    class ScriptSystem : public ISystem<ScriptComponent> {
    public:
        enum class ScriptSystemType {
            Sol2
        };
        ScriptSystem(ScriptSystemType type = ScriptSystemType::Sol2);
        void initialize() override;
        void update(float deltaTime) override;
        void shutdown() override;
        void registerComponent(const ComponentPtr component) override;
        void deregisterComponent(const ComponentPtr component) override;

        IScriptEngineState* getEngineState() const { return scriptEngineState.get(); }
    private:
        ScriptSystemType systemType;
        std::unique_ptr<IScriptEngineState> scriptEngineState;
    };
} // namespace LaurelEye
