emitter = nil
isPlaying = true

function onStart()

    emitter = self:findParticleEmitter()

    if emitter == nil then
        logerr("[ParticleToggle] No ParticleEmitter found on entity!")
        return
    end

    emitter:play()
    log("[ParticleToggle] Emitter initialized and playing.")

end

function onUpdate(dt)

    if Input:isKeyPressed(Key.P) then
        emitter:toggle()
    end

end

function onShutdown()
    log("[ParticleToggle] Script shutdown.")
end
