#include "LaurelEyeEngine/scripting/sol2/bindings/Sol2API_Window.h"

#include "LaurelEyeEngine/core/EngineContext.h"
#include "LaurelEyeEngine/window/IWindow.h"
#include "LaurelEyeEngine/window/WindowManager.h"

namespace LaurelEye::Scripting {
    void Sol2API_Window::setup(sol::state& lua, EngineContext* ctx) {
        sol::table Window = lua["Window"].get_or_create<sol::table>();

        Window.new_enum<CursorMode>(
            "CursorMode",
            {
                {  "Normal",   CursorMode::Normal},
                {  "Hidden",   CursorMode::Hidden},
                {"Disabled", CursorMode::Disabled},
                {"Captured", CursorMode::Captured}
        });

        WindowManager* wm = ctx->getService<WindowManager>();

        Window.set_function(
            "setCursorMode",
            [wm](int windowId, CursorMode mode) -> void {
                wm->setCursorMode(windowId, mode);
            });

        Window.set_function(
            "setFullscreen",
            [wm](int windowId, bool fullscreen) -> void {
                wm->setFullscreen(windowId, fullscreen);
            });
    }

} // namespace LaurelEye::Scripting
