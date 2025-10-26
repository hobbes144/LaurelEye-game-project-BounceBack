#include "LaurelEyeEngine/scripting/sol2/Sol2API.h"
#include <iostream>
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Math.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Input.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Time.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Physics.h"

namespace LaurelEye::Scripting {

    void Sol2API::registerState(sol::state& lua) {
        /* Logging*/
        lua.set_function("log", log);
        lua.set_function("logErr", logErr);
        lua.set_function("logWarn", logWarn);

        /* Time */

        /* Math */
        Sol2API_Math::setup(lua);

        /* Input*/

        /* Renderer*/

        /* Physics */
    }

    void Sol2API::registerEnvironment(sol::environment& env, LaurelEye::Entity* owner) {
        // Make 'self' available to scripts
        env["self"] = owner;

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
