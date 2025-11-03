-- Scene Manager Controller
-- Switches scenes based on key inputs

function onStart()
    log("Scene Manager script started")
end

function onUpdate(dt)
    -- Example key bindings for switching scenes
    if Input:isKeyPressed(Key.One) then
        log("Loading Scene: Level1")
        SceneManager:changeScene("Level1")
    elseif Input:isKeyPressed(Key.Two) then
        log("Loading Scene: Level2")
        SceneManager:changeScene("Level2")
    elseif Input:isKeyPressed(Key.Three) then
        log("Loading Scene: PhysicsScene")
        SceneManager:changeScene("PhysicsScene")
    elseif Input:isKeyPressed(Key.Four) then
        log("Loading Scene: ParticleScene")
        SceneManager:changeScene("ParticleScene")
    elseif Input:isKeyPressed(Key.Five) then
        log("Reloading Current Scene")
        SceneManager:rechangeScene()
    end
end

function onShutdown()
    log("Scene Manager script stopped")
end
