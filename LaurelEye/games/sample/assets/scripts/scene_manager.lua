-- Scene Manager Controller
-- Switches scenes based on key inputs

local fullscreen = false
local hideMouse = true;

function onStart()
    -- WHY IS EVERY SCRIPT CONSTANTLY RUNNING???
    -- Window.setCursorMode(0, Window.CursorMode.Normal)

    if Serialized.hideMouse ~= nil then
        hideMouse = Serialized.hideMouse
    end

    if hideMouse then
        Window.setCursorMode(0, Window.CursorMode.Disabled)
    else 
        Window.setCursorMode(0, Window.CursorMode.Normal)
    end

end

function onUpdate(dt)
    if Input:isKeyPressed(Key.One) then
        log("Loading Scene: Level1")
        Window.setCursorMode(0, Window.CursorMode.Disabled)
        SceneManager:changeScene("Level1")
    elseif Input:isKeyPressed(Key.Two) then
        log("Loading Scene: Level2")
        Window.setCursorMode(0, Window.CursorMode.Disabled)
        SceneManager:changeScene("Level2")
    elseif Input:isKeyPressed(Key.Three) then
        log("Loading Scene: Level3")
        Window.setCursorMode(0, Window.CursorMode.Disabled)
        SceneManager:changeScene("Level3")
    elseif Input:isKeyPressed(Key.Four) then
        log("Loading Scene: Level4")
        Window.setCursorMode(0, Window.CursorMode.Disabled)
        SceneManager:changeScene("Level4")
    elseif Input:isKeyPressed(Key.Five) then
        log("Loading Scene: Level5")
        Window.setCursorMode(0, Window.CursorMode.Disabled)
        SceneManager:changeScene("Level5")
    elseif Input:isKeyPressed(Key.Escape) then
        Window.setCursorMode(0, Window.CursorMode.Normal)
        SceneManager:changeScene("MainMenu")
    elseif Input:isKeyPressed(Key.Nine) then
        Engine:stop()
    elseif Input:isKeyPressed(Key.F11) then
        fullscreen = not fullscreen
        Window.setFullscreen(0, fullscreen)
    end
end

function onShutdown()
    -- Window.setCursorMode(0, Window.CursorMode.Disabled)
    log("Scene Manager script stopped")
end
