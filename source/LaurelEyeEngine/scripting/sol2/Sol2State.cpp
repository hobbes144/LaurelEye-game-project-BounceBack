#include "LaurelEyeEngine/scripting/sol2/Sol2State.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptInstance.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2API.h"

namespace LaurelEye::Scripting {

    void Sol2State::initialize(EngineContext* ctx) {
        lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::package, sol::lib::debug, sol::lib::string);
        Sol2API::registerState(lua, ctx);
    }

    void Sol2State::shutdown(EngineContext* ctx) {
        Sol2API::shutdown(ctx);
        lua.collect_garbage();
    }

    std::unique_ptr<IScriptInstance> Sol2State::createInstance(const std::string& path, LaurelEye::Entity* owner) {
        return std::make_unique<Sol2ScriptInstance>(lua, path, owner);
    }

} // namespace LaurelEye::Scripting
