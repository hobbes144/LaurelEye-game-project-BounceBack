lifeTime = 3.0          -- total lifespan before auto-destruction
destroyed = false

timeAlive = 0.0
transform = nil
body = nil

function onStart()
    transform = self:findTransform()
    body = self:findPhysics()
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

    local other = data.entityB
    local name = other:getName()
end

function destroySelf()
    if destroyed then return end
    destroyed = true

    -- Optional: spawn particle or sound
    -- SceneManager:instantiate("prefabs/hit_effect.json")

    SceneManager:destroy(self)
end
