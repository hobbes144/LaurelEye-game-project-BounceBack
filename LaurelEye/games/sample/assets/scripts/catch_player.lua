baseSpeed      = 50.0
sprintSpeed    = 75.0
moveSpeed      = baseSpeed
dashSpeed      = 150.0
turnSpeed      = 10.0
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
projectileSpeed = 100.0

--damage variables
maxHealth = 3.0
currentHealth = maxHealth
invincible = false
invincibleTimer = 0.0
invincibleDuration = 1.5

dashDuration = 0.25
dashTimer = 0.0
isDashing = false

function onStart()
    transform = self:findTransform()
    body = self:findRigidBody()

    animator = self:findAnimator()
    if animator == nil then return end
    animator:changeAnimation("StickMan_Idle")

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
end

function onMessage(msg)
    if msg.topic == "Catch Me!" then
        hasBall = true
    end

    if msg.topic == "Get Hit!" then
        if not invincible then
            currentHealth = currentHealth - 1.0
            invincible = true
            moveHealthBar()
        end
    end
end

function onUpdate(dt)
    if cameraTransform == nil then
        local scene = SceneManager:getCurrentScene()
        if scene ~= nil then
            local cameraEntity = scene:findEntityByName("Camera")
            if cameraEntity ~= nil then
                print("Camera Entity: ", cameraEntity)
                cameraTransform = cameraEntity:findTransform()
                print("Camera Transform: ", cameraTransform)
            end
        end
    end

    --Check if health = 0
    if currentHealth <= 0 then
        log("Player has died!")
        SceneManager:changeScene("MainMenu")
    end

    if invincible then
        invincibleTimer = invincibleTimer + dt
        if invincibleTimer >= invincibleDuration then
            invincible = false
        end
    end

    if isDashing then
        dashTimer = dashTimer + dt
        if dashTimer >= dashDuration then
            isDashing = false

            -- Only disable if not in damage invincibility
            if invincibleTimer >= dashDuration then
                invincible = false
            end
        end
    end

    moveSpeed = Input:isKeyHeld(Key.LShift) and sprintSpeed or baseSpeed

    -- Input
    local lStickX = Input:getGamepadAxis(GamepadAxes.LStickX)
    local lStickY = Input:getGamepadAxis(GamepadAxes.LStickY)

    -- Deadzone
    if math.abs(lStickX) < 0.5 then lStickX = 0 end
    if math.abs(lStickY) < 0.5 then lStickY = 0 end

    local inputX =
        lStickX +
        (Input:isKeyHeld(Key.D) and 1 or 0) -
        (Input:isKeyHeld(Key.A) and 1 or 0)

    local inputZ =
        -lStickY +
        (Input:isKeyHeld(Key.W) and 1 or 0) -
        (Input:isKeyHeld(Key.S) and 1 or 0)

    -- Normalize input
    local inputMag = math.sqrt(inputX * inputX + inputZ * inputZ)
    if inputMag > 1 then
        inputX = inputX / inputMag
        inputZ = inputZ / inputMag
    end

    -- Camera Movement
    local moveDir = getCameraRelativeMovement(inputX, inputZ)

    -- Physics Movement
    local vel = body:getLinearVelocity()
    local mass = body:getBodyData().mass

    local desiredX = moveDir.x * moveSpeed
    local desiredZ = moveDir.z * moveSpeed

    local dvx = desiredX - vel.x
    local dvz = desiredZ - vel.z
    local dvMag = math.sqrt(dvx * dvx + dvz * dvz)

    if dvMag > dvDeadzone then

        local timeToReach = isGrounded and timeToReachGround or timeToReachAir

        local accelX = dvx / math.max(timeToReach, 0.0001)
        local accelZ = dvz / math.max(timeToReach, 0.0001)

        local accelMag = math.sqrt(accelX * accelX + accelZ * accelZ)

        body:applyForce(Vector3.new(accelX * mass, 0, accelZ * mass))
        local animName = animator.currentAnimationName
        if animName ~= "StickMan_Running" then
            animator:changeAnimation("StickMan_Running")
        end
    else
        local animName = animator.currentAnimationName
        if animName ~= "StickMan_Idle" then
            animator:changeAnimation("StickMan_Idle")
        end
    end

    -- Rotation
    if inputMag > 0 then
        local angle = math.atan(moveDir.x, moveDir.z)
        rotateTo(angle, dt)
    end

    if Input:isMouseButtonPressed(MouseButton.Left) and hasBall then
        shootProjectile()
        hasBall = false
    end

    if Input:isKeyPressed(Key.Space) then
        isDashing = true
        dashTimer = 0.0

        invincible = true
        invincibleTimer = 0.0

        local dashX = moveDir.x * dashSpeed
        local dashZ = moveDir.z * dashSpeed

        local dx = dashX - vel.x
        local dz = dashZ - vel.z

        body:applyImpulse(Vector3.new(dx * mass, 0, dz * mass)) 
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
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end

    local playerTransform = self:findTransform()
    if playerTransform == nil then return end

    -- projectile spawn (player / weapon space)
    local spawnHeight = 8.0
    local spawnForward = 4.0

    local playerPos = playerTransform:getWorldPosition()
    local aimDir = findForward()

    local spawnPos =
        playerPos
        + Vector3.new(0, spawnHeight, 0)
        + aimDir * spawnForward

    -- get camera-based aim target
    local targetPoint = cameraAim(1000.0)
    if targetPoint == nil then return end

    local shootDir = (targetPoint - spawnPos)
    if shootDir:Magnitude() <= 0.0001 then return end
    shootDir = shootDir:Normalized()

    proj = SceneManager:instantiate("prefabs/catch_ball.prefab.json")
    if proj == nil then return end
    local projTransform = proj:findTransform()
    if projTransform == nil then return end
    projTransform:setWorldPosition(spawnPos)

    local projBody = proj:findGhostBody()
    if projBody ~= nil then
        print("Shooting Projectile")
        projBody:setLinearVelocity(shootDir * projectileSpeed)
    end
end

function onCollisionEnter(data)
    --Check for enemy collision
    local tagsA = data.entityA:getTags()
    for _, tag in pairs(tagsA) do
        if tag == "ground" then
            if not isGrounded then
                triggerLandingBurst()
            end
            isGrounded = true
            jumping = false
        elseif tag == "door" then
            SceneManager:destroy(data.entityB)
            changeLevels()
        end
    end

    local tagsB = data.entityB:getTags()
    for _, tag in pairs(tagsB) do
        if tag == "ground" then
            if not isGrounded then
                triggerLandingBurst()
            end
            isGrounded = true
            jumping = false
        elseif tag == "door" then
            SceneManager:destroy(data.entityB)
            changeLevels()
        end
    end

end

function catchBall(ball)
    local ballBody = ball:findGhostBody()
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

function moveHealthBar()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local healthBar = scene:findEntityByName("HealthBar")
    local barTransform = healthBar:findComponent("UITransformComponent")
    local hSize = barTransform:getSize()
    local maxSize = 160
    local percent = currentHealth / maxHealth
    local newSize = Vector2.new(maxSize * percent, hSize.y)
    barTransform:setSize(newSize)
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
        log("Alpha Completed!")
    end
end

function cameraAim(maxDistance)
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return nil end

    local cam = scene:findEntityByName("Camera")
    if cam == nil then return nil end

    local camTransform = cam:findTransform()
    if camTransform == nil then return nil end

    local camPos = camTransform:getWorldPosition()
    local forward = camTransform:getWorldRotation():forward():Normalized()

    local hit = Physics.Raycast(camPos, forward, maxDistance, { layerMask = Layers.World | Layers.Enemy })

    if hit ~= nil then
        return hit.position
    end

    return camPos + forward * maxDistance
end
