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
#include "LaurelEyeEngine/math/Vector3.h"
#include "LaurelEyeEngine/transform/TransformComponent.h"
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>

namespace LaurelEye::Scripting {

    struct ScriptValue;

    using ScriptArray = std::vector<ScriptValue>;
    using ScriptObject = std::unordered_map<std::string, ScriptValue>;

    using ScriptVariant = std::variant<std::monostate, bool, int, float, double, std::string,
                                       ScriptArray, ScriptObject,
                                       Vector2, Vector3, Vector4,
                                       Entity*, TransformComponent*>;

    struct ScriptValue {

        ScriptVariant data;

        ScriptValue() = default;

        template<typename T>
        ScriptValue(T value) : data(value){}

        template<typename T>
        bool is() const {
            return std::holds_alternative<T>(data);
        }

        template<typename T>
        const T& get() const {
            return std::get<T>(data);
        }

        //const ScriptVariant& getRaw() const { return data; }

    };

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

        void setSerializedValue(const std::string& name, const ScriptValue& value) {
            serializedData[name] = value;
        }

        bool hasSerializedValue(const std::string& name) const {
            return serializedData.find(name) != serializedData.end();
        }

        ScriptValue* getSerializedValue(const std::string& name) {
            auto it = serializedData.find(name);

            if ( it == serializedData.end() )
                return nullptr;

            return &it->second;
        }

        const std::unordered_map<std::string, ScriptValue>& getAllSerializedData() const {
            return serializedData;
        }

    private:
        std::string scriptPath;
        std::unique_ptr<IScriptInstance> scriptInstance;

        std::unordered_map<std::string, ScriptValue> serializedData;
    };
} //LaurelEye::Scripting
