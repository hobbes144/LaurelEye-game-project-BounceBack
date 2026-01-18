-- Scene Manager Controller
-- Switches scenes based on key inputs

function onStart()
    log("Scene Manager script started")
end

function onUpdate(dt)
    if Input:isKeyPressed(Key.Escape) then
        Engine:stop()
    end

end

function onShutdown()
    log("Scene Manager script stopped")
end
