#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptInstance.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2API.h"

namespace LaurelEye::Scripting {

    Sol2ScriptInstance::Sol2ScriptInstance(sol::state& luaState, const std::string& scriptPath, LaurelEye::Entity* owner_)
        : lua(luaState), env(luaState, sol::create, luaState.globals()), owner(owner_) {

        Sol2API::registerEnvironment(env, owner);

        try {
            lua.script_file(scriptPath, env);
        }
        catch ( const sol::error& e ) {
            std::cerr << "[Lua] script_file error: " << e.what() << std::endl;
        }

        startFunc = env["onStart"];
        updateFunc = env["onUpdate"];
        shutdownFunc = env["onShutdown"];
    }

    void Sol2ScriptInstance::onStart() {
        if ( startFunc.valid() ) startFunc();
    }

    void Sol2ScriptInstance::onUpdate(float dt) {
        if ( updateFunc.valid() ) updateFunc(dt);
    }

    void Sol2ScriptInstance::onShutdown() {
        if ( shutdownFunc.valid() ) shutdownFunc();
    }

} // namespace LaurelEye::Scripting
