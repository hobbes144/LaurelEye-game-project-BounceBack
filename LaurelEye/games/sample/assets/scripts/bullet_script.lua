lifeTime = 3.0          -- total lifespan before auto-destruction
destroyed = false

timeAlive = 0.0
transform = nil
body = nil

speaker = nil

function onStart()
    transform = self:findTransform()
    body = self:findPhysics()

    speaker = self:findAudioSpeaker()
    if speaker ~= nil then
        speaker:stop()
        speaker:play()
    end
end

function onUpdate(dt)
    if destroyed then return end

    timeAlive = timeAlive + dt

    -- Auto-destroy
    if timeAlive >= lifeTime then
        destroySelf()
    end
end

function onCollisionEnter(data)
    if destroyed then return end

    
end

function destroySelf()
    if destroyed then return end
    destroyed = true

    SceneManager:destroy(self)
end
