/// @file   ScriptComponent.h
/// @author Alex Humphries (humphriesa02@gmail.com)
/// @par    **DigiPen Email**
///     alex.humphries@digipen.edu
/// @date   9-24-2025
/// @brief  Holds a unique pointer to a script instance, and remembers the script path.
/// Will likely be set and forgotten, as scripts can usually handle themselves. Only really
/// needed if one wants to swap out a script instance.
///
/// Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.

#pragma once
#include "LaurelEyeEngine/ecs/IComponent.h"
#include "LaurelEyeEngine/scripting/IScriptInstance.h"
#include <memory>
#include <string>

namespace LaurelEye::Scripting {
    class ScriptComponent : public IComponent {
    public:
        ScriptComponent() : scriptPath("") {}
        explicit ScriptComponent(std::string path)
            : scriptPath(path) {}

        const std::string& getScriptPath() const { return scriptPath; }
        IScriptInstance* getScriptInstance() const { return scriptInstance.get(); }
        void destroyScriptInstance() { scriptInstance.reset(); }

        void setScriptInstance(std::unique_ptr<IScriptInstance> instance) {
            scriptInstance = std::move(instance);
        }
    private:
        std::string scriptPath;
        std::unique_ptr<IScriptInstance> scriptInstance;
    };
} //LaurelEye::Scripting
