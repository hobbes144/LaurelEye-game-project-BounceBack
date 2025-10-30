#include "LaurelEyeEngine/scripting/sol2/Sol2API.h"
#include <iostream>
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Math.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Input.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Time.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Physics.h"

namespace LaurelEye::Scripting {

    void Sol2API::registerState(sol::state& lua, EngineContext* ctx) {
        /* Logging*/
        lua.set_function("log", log);
        lua.set_function("logErr", logErr);
        lua.set_function("logWarn", logWarn);

        /* Time */

        /* Math */
        Sol2API_Math::setup(lua);

        /* Input*/
        Sol2API_Input::setup(lua, ctx);

        /* Renderer*/

        /* Physics */
    }

    void Sol2API::registerEnvironment(sol::environment& env, LaurelEye::Entity* owner) {
        // Make 'self' available to scripts
        env["self"] = owner; // you can keep this raw pointer for now

        // Optional: you can override env-specific things here later
    }

    void Sol2API::log(const std::string& msg) {
        std::cout << msg << std::endl;
    }

    void Sol2API::logErr(const std::string& msg) {
        std::cerr << "Error: " << msg << std::endl;
    }

    void Sol2API::logWarn(const std::string& msg) {
        std::cout << "Warning: " << msg << std::endl;
    }

} // namespace LaurelEye::Scripting
