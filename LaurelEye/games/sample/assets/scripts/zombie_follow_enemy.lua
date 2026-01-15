maxSpeed    = 35.0       -- max units per second
accel       = 20.0      -- acceleration toward target
decel       = 10.0      -- deceleration when stopping
chaseRange  = 1000.0      -- distance to start chasing
stopRange   = 0.2       -- distance to stop

turnSpeed   = 8.0       -- how fast the enemy rotates toward movement

transform = nil
body = nil
destroyed = false

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
    local target = findPlayer()
    if not target then return end

    local pos = transform:getWorldPosition()
    local dir = target - pos
    dir.y = 0.0


    local dist = dir:Magnitude()
    if dist <= chaseRange then
        local n = dir:Normalized()

        local vel = body:getLinearVelocity()
        local desiredVel = n * maxSpeed
        local ax = (desiredVel.x - vel.x) * accel * dt
        local az = (desiredVel.z - vel.z) * accel * dt

        vel.x = vel.x + ax
        vel.z = vel.z + az
        local horizSq = vel.x*vel.x + vel.z*vel.z
        if horizSq > maxSpeed * maxSpeed then
            local m = math.sqrt(horizSq)
            vel.x = (vel.x / m) * maxSpeed
            vel.z = (vel.z / m) * maxSpeed
        end

        body:setLinearVelocity(vel)

        -- Rotation
        local horizMag = math.sqrt(vel.x*vel.x + vel.z*vel.z)
        if horizMag > 0.05 then
            local angle = math.atan(vel.x, vel.z)
            rotateTo(angle, dt)
        end
    elseif dist <= stopRange then
        -- Stop smoothly
        local vel = body:getLinearVelocity()
        vel.x = vel.x * 0.8
        vel.z = vel.z * 0.8
        body:setLinearVelocity(vel)
        return
    end
end

function rotateTo(angle, dt)
    local targetQuat = Quaternion.fromEuler(0, angle, 0)
    local currentRot = transform:getWorldRotation()
    local newRot = Quaternion.slerp(currentRot, targetQuat, dt * turnSpeed)
    transform:setWorldRotation(newRot)
end

function onCollisionEnter(data)
    local tagsA = data.entityA:getTags()
    for _, tag in pairs(tagsA) do
        if tag == "bullet" then
            print("Collided with Bullet!")
            if destroyed then return end
            destroySelf()
        end
    end

    local tagsB = data.entityB:getTags()
    for _, tag in pairs(tagsB) do
        if tag == "bullet" then
            print("Collided with Bullet!")
            if destroyed then return end
            destroySelf()
        end
    end
    
    
end
function onCollisionStay(data) end
function onCollisionExit(data) end

function findPlayer()
    local scene = SceneManager:getCurrentScene()
    if not scene then return nil end

    local playerEntity = scene:findEntityByName("PlayerPrefab")
    if not playerEntity then return nil end

    local playerTransform = playerEntity:findTransform()
    if not playerTransform then return nil end

    return playerTransform:getWorldPosition()
end

function destroySelf()
    if destroyed then return end
    destroyed = true
    print(speaker)
    if speaker ~= nil then
        print("speaker noise")
        speaker:stop("explosion")
        speaker:play("explosion")
    end
    SceneManager:destroy(self)
end
