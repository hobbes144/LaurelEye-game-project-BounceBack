lifeTime = 10.0          -- total lifespan before auto-destruction
destroyed = false

timeAlive = 0.0
transform = nil
body = nil

speaker = nil

function onStart()
    transform = self:findTransform()
    body = self:findRigidBody()
    speaker = self:findAudioSpeaker()
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

    local a = data.entityA
    local b = data.entityB
    
    local isProjA = a:compareTag("projectile")
    local isProjB = b:compareTag("projectile")

    local isPlayerA = a:compareTag("player")
    local isPlayerB = b:compareTag("player")

    -- Player collision logic
    if isPlayerA or isPlayerB then
        print("Collided with Player!")
        destroySelf()
        return
    end

    -- Perfect elastic collision between two projectile-tagged bodies
    if isProjA and isProjB then
        local bodyA = a:findRigidBody()
        local bodyB = b:findRigidBody()
        if not bodyA or not bodyB then return end

        -- Get THEIR velocities before collision impulse
        local vA = bodyA:getLinearVelocity()
        local vB = bodyB:getLinearVelocity()

        -- Swap for perfectly elastic equal-mass collision
        bodyA:setLinearVelocity(vA)
        bodyB:setLinearVelocity(vB)

        if speaker ~= nil then
            speaker:play()
        end

        return
    end
end

function destroySelf()
    if destroyed then return end
    destroyed = true
    SceneManager:destroy(self)
end
