#include "LaurelEyeEngine/scripting/sol2/Sol2State.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptInstance.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2API.h"

namespace LaurelEye::Scripting {

    void Sol2State::initialize() {
        lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::package);
        Sol2API::registerState(lua);
    }

    void Sol2State::shutdown() {
        lua.collect_garbage();
    }

    std::unique_ptr<IScriptInstance> Sol2State::createInstance(const std::string& path, LaurelEye::Entity* owner) {
        return std::make_unique<Sol2ScriptInstance>(lua, path, owner);
    }

} // namespace LaurelEye::Scripting
