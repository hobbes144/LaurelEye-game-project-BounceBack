-- Scene Manager Controller
-- Switches scenes based on key inputs

function onStart()
end

function onUpdate(dt)
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
        log("Loading Scene: SpawnerScene")
        SceneManager:changeScene("SpawnerScene")
    elseif Input:isKeyPressed(Key.Six) then
        log("Reloading Current Scene")
    elseif Input:isKeyPressed(Key.Eight) then
        log("Loading Scene: BulletHellPrototype")
        SceneManager:changeScene("BulletHellPrototype")
        SceneManager:reloadCurrentScene()
    elseif Input:isKeyPressed(Key.Seven) then
        log("Loading Scene: MBPrototype")
        SceneManager:changeScene("MBPrototype")
    elseif Input:isKeyPressed(Key.Escape) then
        SceneManager:changeScene("MainMenu")
    elseif Input:isKeyPressed(Key.Nine) then
        Engine:stop()
    end

end

function onShutdown()
    log("Scene Manager script stopped")
end
