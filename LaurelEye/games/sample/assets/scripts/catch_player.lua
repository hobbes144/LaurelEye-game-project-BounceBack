maxSpeed       = 200.0
accelGround    = 250.0
accelAir       = 30.0
decelGround    = 5.0
jumpVelocity   = 60.5
jumpCut        = 0.45
turnSpeed      = 10.0
dustTimer = 0.0
dustInterval = 0.05
timeToReachGround = 0.12   -- seconds to reach desired speed on ground (tweak)
timeToReachAir    = 0.25   -- seconds to reach desired speed in air
dvDeadzone        = 0.05   -- ignore tiny dv values to avoid jitter

transform = nil
body = nil
cameraTransform = nil
smokeEmitter = nil
speaker = nil

isGrounded = true
jumping = false

landingBurstTime = 0.1
landingBurstTimer = 0.0
inLandingBurst = false

defaultEmissionRate = 150.0
defaultSpreadAngle = 60.0
defaultStartSize = 20.0
defaultEndSize = 10.0

walkSoundMinSpeed = 5.0
isWalkSoundPlaying = false

footstepTimer = 0.0
stepInterval = 0.35   -- seconds between steps
speedThreshold = 2.0  -- when footsteps should begin

hasBall = true
ballCreated = false

--damage variables
health = 3.0
invincible = false
invincibleTimer = 0

-- Health UI Elements
healthUIElements = {}

doorSpawned = false

function onStart()
    transform = self:findTransform()
    body = self:findPhysics()
    local scene = SceneManager:getCurrentScene()
    if scene ~= nil then
        local cameraEntity = scene:findEntityByName("Camera")
        if cameraEntity ~= nil then
            cameraTransform = cameraEntity:findTransform()
        end
    end
    
    smokeEmitter = self:findParticleEmitter()

    if smokeEmitter ~= nil then
        smokeEmitter:pause()

        local data = smokeEmitter:getEmitterData()
        defaultEmissionRate = data.emissionRate
        defaultStartSize    = data.startSize
        defaultEndSize      = data.endSize
        defaultSpreadAngle  = data.spreadAngle
    end

    speaker = self:findAudioSpeaker()

    -- Assume there are 3 health UI elements; adjust count as needed
    for i = 1, 3 do
        healthUIElements[i] = UIElementManager:findUIComponent("HealthUI" .. i)
    end
end

function onUpdate(dt)
    --Check if health = 0
    if health <= 0 then
        print("Player has died!")
        SceneManager:changeScene("MainMenu")
    end

    for i, uiElement in ipairs(healthUIElements) do
        if uiElement then
            if i <= health then
                uiElement:setActive(true)
            else
                uiElement:setActive(false)
            end
        end
    end

    if invincible then
        invincibleTimer = invincibleTimer + dt
        if invincibleTimer >= 1.5 then
            invincible = false
            invincibleTimer = 0.0
        end
    end

    -- Poll input
    local lStickX = Input:getGamepadAxis(GamepadAxes.LStickX)
    local lStickY = Input:getGamepadAxis(GamepadAxes.LStickY)

    if lStickX < 0.5 and lStickX > -0.5 then
        lStickX = 0.0
    end
    if lStickY < 0.5 and lStickY > -0.5 then
        lStickY = 0.0
    end
    local inputX = lStickX +
                   (Input:isKeyHeld(Key.D) and 1 or 0) +
                   (Input:isKeyHeld(Key.A) and -1 or 0)

    local inputZ = -lStickY +
                   (Input:isKeyHeld(Key.S) and -1 or 0) +
                   (Input:isKeyHeld(Key.W) and 1 or 0)

    -- Clamp input to [-1, 1] range
    inputX = math.max(-1, math.min(1, inputX))
    inputZ = math.max(-1, math.min(1, inputZ))

    if hasBall and Input:isMouseButtonPressed(MouseButton.Left) then
        shootProjectile()
        hasBall = false
    end

    local mag = math.sqrt(inputX*inputX + inputZ*inputZ)
    if mag > 0 then
        inputX = inputX / mag
        inputZ = inputZ / mag
    end

    local moveDir = getCameraRelativeMovement(inputX, inputZ)
    local m = math.sqrt(moveDir.x*moveDir.x + moveDir.z*moveDir.z)

    -- Normalize
    if m < 0.0001 then
        moveDir.x = 0
        moveDir.z = 0
    else
        moveDir.x = moveDir.x / m
        moveDir.z = moveDir.z / m
    end

    local vel = body:getLinearVelocity()
    local mass = body:getBodyData().mass
    local desiredX = moveDir.x * maxSpeed
    local desiredZ = moveDir.z * maxSpeed

    -- horizontal delta velocity
    local dvx = desiredX - vel.x
    local dvz = desiredZ - vel.z
    local dvMag = math.sqrt(dvx*dvx + dvz*dvz)

    -- small deadzone to avoid jitter
    if dvMag > dvDeadzone then
        -- choose time constant depending on grounded state
        local timeToReach = isGrounded and timeToReachGround or timeToReachAir

        -- desired acceleration to reach the target within timeToReach
        local accelReqX = dvx / math.max(timeToReach, 0.0001)
        local accelReqZ = dvz / math.max(timeToReach, 0.0001)
        local accelReqMag = math.sqrt(accelReqX*accelReqX + accelReqZ*accelReqZ)

        -- clamp to maximum allowed acceleration (ground/air)
        local maxAccel = isGrounded and accelGround or accelAir
        if accelReqMag > maxAccel then
            accelReqX = accelReqX / accelReqMag * maxAccel
            accelReqZ = accelReqZ / accelReqMag * maxAccel
        end

        -- force = mass * accel (physics: F = m * a)
        local force = Vector3.new(accelReqX * mass, 0, accelReqZ * mass)
        body:applyImpulse(force * dt)
    end

    -- Jump check
    if isGrounded and (Input:isKeyPressed(Key.Space) or Input:isButtonPressed(GamepadButton.A)) then
        local mass = body:getBodyData().mass
        body:applyImpulse(Vector3.new(0, jumpVelocity * mass, 0))
        jumping = true
        isGrounded = false
    end

    -- Jump cut
    if jumping and not (Input:isKeyHeld(Key.Space) or Input:isButtonHeld(GamepadButton.A)) and vel.y > 0 then
        vel.y = vel.y * jumpCut
        body:setLinearVelocity(vel)
        jumping = false
    end

    -- Rotate
    if mag > 0 then
    local angle = math.atan(moveDir.x, moveDir.z)
    rotateTo(angle, dt)
    end

    -- Dust + Landing Plume
    local v = body:getLinearVelocity()

    if smokeEmitter ~= nil and not inLandingBurst then
        local horizontalSpeed = math.sqrt(v.x*v.x + v.z*v.z)

        if isGrounded and horizontalSpeed > 50.0 then
            dustTimer = dustTimer - dt
            if dustTimer <= 0.0 then
                smokeEmitter:play()
                dustTimer = dustInterval
            end
        else
            dustTimer = dustInterval
            smokeEmitter:pause()
        end
    end

    if inLandingBurst then
        landingBurstTimer = landingBurstTimer - dt
        if landingBurstTimer <= 0 then
            local data = smokeEmitter:getEmitterData()
            data.emissionRate = defaultEmissionRate
            data.startSize = defaultStartSize
            data.endSize = defaultEndSize
            data.spreadAngle = defaultSpreadAngle
            inLandingBurst = false
        end
    end

    if speaker ~= nil then
        local horizontalSpeed = math.sqrt(v.x*v.x + v.z*v.z)

        if isGrounded and horizontalSpeed > walkSoundMinSpeed then
            footstepTimer = footstepTimer + dt
            if footstepTimer >= stepInterval then
                footstepTimer = 0.0

                if speaker then
                    speaker:stop()
                    speaker:play()
                end
            end
        else
            -- stopped or slow: reset timer
            footstepTimer = 0.0
        end
    end

    if hasBall then
        local scene = SceneManager:getCurrentScene()
        if scene == nil then return end
        local ball = scene:findEntityByName("Ball")
        if ball == nil then return end
        local bTransform = ball:findTransform()
        if bTransform == nil then return end
        local aimDir = findForward()

        local playerTransform = self:findTransform()
        if playerTransform == nil then return end
        local playerPos = playerTransform:getWorldPosition()
        local spawnHeight = 4.0
        local spawnForward = 4.0

        local spawnPos =
            playerPos
            + Vector3.new(0, spawnHeight, 0)
            + aimDir * spawnForward

        bTransform:setWorldPosition(spawnPos)
    end

    local anyTargets = targetCheck()
    if not doorSpawned and not anyTargets then
        SceneManager:instantiate("prefabs/door.prefab.json")
        doorSpawned = true
    end
end

function triggerLandingBurst()
    if smokeEmitter == nil then return end

    local data = smokeEmitter:getEmitterData()
    data.emissionRate = defaultEmissionRate * 2.0
    data.startSize = defaultStartSize * 2.5
    data.endSize = defaultEndSize * 2.0
    data.spreadAngle = 90.0

    smokeEmitter:playFor(0.2)

    landingBurstTimer = landingBurstTime
    inLandingBurst = true
end

function getCameraRelativeMovement(inputX, inputZ)
    if cameraTransform == nil then
        return Vector3.new(inputX, 0, inputZ)
    end

    local camRot = cameraTransform:getWorldRotation()
    local forward = camRot:forward()
    forward.y = 0
    if forward:Magnitude() > 0 then
        forward = forward:Normalized()
    end

    local right = camRot:right()
    right.y = 0
    if right:Magnitude() > 0 then
        right = right:Normalized()
    end

    local moveDir = forward * inputZ + right * inputX
    if moveDir:Magnitude() > 0 then
        moveDir = moveDir:Normalized()
    end
    return moveDir
end

function rotateTo(angle, dt)
    local target = Quaternion.fromEuler(0, angle, 0)
    local current = transform:getWorldRotation()
    local rot = Quaternion.slerp(current, target, dt * turnSpeed)
    transform:setWorldRotation(rot)
end

function shootProjectile()
    if self == nil then return end

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end

    -- Player transform
    local playerTransform = self:findTransform()
    if playerTransform == nil then return end

    -- Camera transform
    local aimDir = findForward()

    -- Spawn position
    local playerPos = playerTransform:getWorldPosition()
    local spawnHeight = 8.0
    local spawnForward = 4.0

    local spawnPos =
        playerPos
        + Vector3.new(0, spawnHeight, 0)
        + aimDir * spawnForward

    -- Instantiate projectile
    local proj = nil
    if not ballCreated then
        proj = SceneManager:instantiate("prefabs/catch_ball.prefab.json")
        if proj == nil then return end
        ballCreated = true
    else
        proj = scene:findEntityByName("Ball")
        if proj == nil then return end
    end

    local projTransform = proj:findTransform()
    if projTransform == nil then return end

    projTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    -- Rotate projectile to face aim direction (yaw only)
    local flat = Vector3.new(aimDir.x, 0, aimDir.z)
    if flat:Magnitude() > 0 then
        flat = flat:Normalized()
        local yaw = math.atan(-flat.x, -flat.z)
        projTransform:setWorldRotation(Quaternion.fromEuler(0, yaw, 0))
    end

    -- Apply velocity
    local projBody = proj:findPhysics()
    if projBody ~= nil then
        local projectileSpeed = 150.0
        projBody:setLinearVelocity(aimDir * projectileSpeed)
    end
end



function onCollisionEnter(data)
    --Check for enemy collision
    local tagsA = data.entityA:getTags()
    for _, tag in pairs(tagsA) do
        if tag == "ball" then
            catchBall(data.entityB)
        elseif tag == "ground" then
            if not isGrounded then
                triggerLandingBurst()
            end
            isGrounded = true
            jumping = false
        elseif tag == "bullet" then
            if not invincible then
                health = health - 1.0
                invincible = true
            end
        elseif tag == "door" then
            changeLevels()
        end
    end

    local tagsB = data.entityB:getTags()
    for _, tag in pairs(tagsB) do
        if tag == "ball" then
            catchBall(data.entityB)
        elseif tag == "ground" then
            if not isGrounded then
                triggerLandingBurst()
            end
            isGrounded = true
            jumping = false
        elseif tag == "bullet" then
            if not invincible then
                health = health - 1.0
                invincible = true
            end
        elseif tag == "door" then
            changeLevels()
        end
    end

end

function onCollisionExit(data)
    local name = data.entityB:getName()
    if name == "Ground" then
        isGrounded = false
    end
end

function catchBall(ball)
    local ballBody = ball:findPhysics()
    ballBody:setLinearVelocity(0.0, 0.0, 0.0)
    hasBall = true
end

function findForward()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    -- Camera transform (your third-person camera)
    local cameraEntity = scene:findEntityByName("Camera")
    if cameraEntity == nil then return end

    local cameraTransform = cameraEntity:findTransform()
    if cameraTransform == nil then return end

    -- Camera aim direction
    local camRot = cameraTransform:getWorldRotation()
    local aimDir = camRot:forward()
    aimDir = aimDir:Normalized()
    return aimDir
end

function targetCheck()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end

    local targetList = scene:findEntitiesWithTag("target")
    if #targetList == 0 then
        return false
    end

    return true
end

function changeLevels()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local sceneName = scene:getName()
    if sceneName == "Level1" then
        SceneManager:changeScene("Level2")
    elseif sceneName == "Level2" then
        SceneManager:changeScene("Level3")
    elseif sceneName == "Level3" then
        print("Alpha Completed!")
    end
    SceneManager:destroy(data.EntityA)
    doorSpawned = false
end
