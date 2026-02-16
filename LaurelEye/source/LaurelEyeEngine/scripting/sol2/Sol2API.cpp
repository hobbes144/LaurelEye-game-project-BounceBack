#include "LaurelEyeEngine/scripting/sol2/Sol2API.h"
#include "LaurelEyeEngine/scripting/ScriptSystem.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2Logging.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Animation.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Audio.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Camera.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_ECS.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Input.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Lights.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Math.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Particles.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Physics.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Renderable.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Scene.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Spline.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Time.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Transform.h"
#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_UI.h"
#include "LaurelEyeEngine/scripting/sol2/Sol2ScriptBroker.h"

namespace LaurelEye::Scripting {

    void Sol2API::registerState(sol::state& lua, EngineContext* ctx) {
        /* Logging*/
        lua.set_function("log", LaurelEye::Log::luaLog);
        lua.set_function("logWarn", LaurelEye::Log::luaLogWarn);
        lua.set_function("logError", LaurelEye::Log::luaLogError);
        lua.set_function("debugLog", LaurelEye::Log::luaDebugLog);
        lua.set_function("debugLogWarn", LaurelEye::Log::luaDebugLogWarn);
        lua.set_function("debugLogError", LaurelEye::Log::luaDebugLogError);

        registerBrokerState(lua, ctx);

        /* Time */

        /* Math */
        Sol2API_Math::setup(lua);

        /* Transform */
        Sol2API_Transform::setup(lua);

        /* Input*/
        Sol2API_Input::setup(lua, ctx);

        /* Scene */
        Sol2API_Scene::setup(lua, ctx);

        /* ECS */
        Sol2API_ECS::setup(lua, ctx);

        /* Renderer*/
        Sol2API_Renderable::setup(lua, ctx);

        /* Camera */
        Sol2API_Camera::setup(lua);

        /* Lights */
        Sol2API_Lights::setup(lua);

        /* Physics */
        Sol2API_Physics::setup(lua, ctx);

        /* Particles*/
        Sol2API_Particles::setup(lua, ctx);

        /* Audio */
        Sol2API_Audio::setup(lua, ctx);

        /* UI */
        Sol2API_UI::setup(lua, ctx);

        /* Spline */
        Sol2API_Spline::setup(lua);

        Sol2API_Animation::setup(lua, ctx);
    }

    void Sol2API::registerEnvironment(sol::environment& env, LaurelEye::Entity* owner) {
        // Make 'self' available to scripts
        env["self"] = owner; // you can keep this raw pointer for now

        // Optional: you can override env-specific things here later
    }

    void Sol2API::shutdown(EngineContext* ctx) {
        // Clean up any global resources if needed
        Sol2API_UI::shutdown(ctx);
    }

    void Sol2API::registerBrokerState(sol::state& lua, EngineContext* ctx) {
        auto broker = ctx->getService<ScriptSystem>()->getBroker();
        static_cast<Sol2ScriptBroker*>(broker)->setupAPI(lua);
    }

} // namespace LaurelEye::Scripting
