-- Scene Manager Controller
-- Switches scenes based on key inputs

local fullscreen = false

function onStart()
    -- WHY IS EVERY SCRIPT CONSTANTLY RUNNING???
    -- Window.setCursorMode(0, Window.CursorMode.Normal)

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
        log("Loading Scene: ParticleScene")
        Window.setCursorMode(0, Window.CursorMode.Disabled)
        SceneManager:changeScene("ParticleScene")
    elseif Input:isKeyPressed(Key.Five) then
        log("Loading Scene: SpawnerScene")
        Window.setCursorMode(0, Window.CursorMode.Disabled)
        SceneManager:changeScene("SpawnerScene")
    elseif Input:isKeyPressed(Key.Six) then
        log("Reloading Current Scene")
        Window.setCursorMode(0, Window.CursorMode.Disabled)
    elseif Input:isKeyPressed(Key.Eight) then
        log("Loading Scene: BulletHellPrototype")
        Window.setCursorMode(0, Window.CursorMode.Disabled)
        SceneManager:changeScene("BulletHellPrototype")
        SceneManager:reloadCurrentScene()
    elseif Input:isKeyPressed(Key.Seven) then
        log("Loading Scene: MBPrototype")
        Window.setCursorMode(0, Window.CursorMode.Disabled)
        SceneManager:changeScene("MBPrototype")
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
