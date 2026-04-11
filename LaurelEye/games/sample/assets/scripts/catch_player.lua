baseSpeed      = 50.0
sprintSpeed    = 75.0
moveSpeed      = baseSpeed
turnSpeed      = 10.0
timeToReachGround = 0.08
timeToReachAir    = 0.25
dvDeadzone        = 0.05

--Kickback anim cooldown
kickbackCountdown = 0.0

--death anim cooldown
deathCooldown = 0.0
dead = false

--getting hit feedback
CreateObj = nil
CreateDamageObj = nil

playerOrgobj = nil
damageObj = nil

damageTex = nil
normalTex = nil

flickerTimer = 0.1
flickerCooldown = 0.0

-----------------------------

---@type Scene|nil
local scene = nil
---@type Transform|nil
local transform = nil
body = nil
---@type Vector3
local playerPos
---@type Vector3
local playerArmPos
---@type Entity|nil
local cameraEntity = nil
---@type Transform|nil
local cameraTransform = nil
smokeEmitter = nil
speaker = nil
audio = nil

isGrounded = true
jumping = false

-- ── Jump settings ─────────────────────────────────────────────────
jumpImpulse       = 50.0
jumpCutMultiplier = 0.45
-- ─────────────────────────────────────────────────────────────────

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
stepInterval = 0.35
speedThreshold = 2.0

---@type boolean
local hasBall = true
ballCreated = false
ballCreated = false
baseProjSpeed = 75.0
projectileSpeed = baseProjSpeed
---@type Vector3
local ballGravity = Vector3.new(0.0, -9.8, 0.0)

maxHealth = 3.0
currentHealth = nil
invincible = false
invincibleTimer = 0.0
invincibleDuration = 1.5

dashDuration = 0.25
dashTimer = 0.0
isDashing = false

kickbackCountdown = 0.0

local TrajectoryLine = require("TrajectoryLine")
---@type TrajectoryLine|nil
local trajectoryLine = nil

hasKey = false






-- projectile spawn (player / weapon space)
---@type Vector3
local spawnHeightVec = Vector3.new(0.0, 8.0, 0.0)
local spawnForward = 4.0
local kickBackHeight = 4.5
---@type Vector3
local spawnPos

---@type Vector3|nil Player aim point
local targetPoint
---@type Vector3
local shootDir

---@type number
local kickBackRange = 20.0

---@type boolean
local isNormalTime = true
---@type number
local slowedTimeScale = 0.06
---@type number
local slowTimeLimit = 1.5
---@type number
local slowCooldown = 0.5
---@type number
local slowTimeLeniency = 0.5
---@type number
local timeSlowLerpSpeed = 16.0
---@type number
local timeScale = 1.0
---@type number
local targetTimeScale = 1.0
---@type number
local slowStartTime
---@type number
local slowEndTime = 0.0
---@type number
local rightMousePressTime = 0.0
---@type number
local timeEps = 0.001

---@class Projectile
---@field entity Entity
---@field position Vector3

--- Get a list of all projectiles that can be kicked back
---@return Projectile[] projList list of projectiles
local function getProjectilesInRange()
    ---@type Scene|nil
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end

    ---@type Vector3
    local initialPos = transform:getWorldPosition()
    ---@type Vector3
    local playerPos = Vector3.new(initialPos.x, initialPos.y + kickBackHeight, initialPos.z)

    ---@type Projectile[]
    local projList = {}

    -- Check the player's ball
    ---@type Entity|nil
    local ball = scene:findEntityByName("Ball")
    if ball ~= nil then
        ---@type Transform|nil
        local ballTransform = ball:findTransform()
        if ballTransform ~= nil then
            ---@type Vector3
            local ballPos = ballTransform:getWorldPosition()
            ---@type number
            local dist = (ballPos - playerPos):Magnitude()

            if dist < kickBackRange then
                -- add sphere that is visible
                -- test if only one ball being kicked back makes more sense
                table.insert(projList, { entity = ball, position = ballPos})
            end
        end
    end

    -- Check all enemy projectiles by tag
    local projectiles = scene:findEntitiesWithTag("bullet")
    for _, projectile in ipairs(projectiles) do
        ---@type Transform|nil
        local projTransform = projectile:findTransform()
        if projTransform ~= nil then
            ---@type Vector3
            local projPos = projTransform:getWorldPosition()
            ---@type number
            local dist = (projPos - playerPos):Magnitude()

            if dist < kickBackRange then
                table.insert(projList, { entity = projectile, position = projPos})
            end
        end
    end

    return projList
end

--- Get the closest projectile from list
---@param projectileList Projectile[]
---@return Projectile|nil
local function getClosestProjectile(projectileList)
    local closestDist = math.huge
    local closest = nil
    for _, projectile in ipairs(projectileList) do
        local projDist = projectile.position:Magnitude()
        if projDist < closestDist then
            closestDist = projDist
            closest = projectile
        end
    end
    return closest
end

function onStart()
    CreateObj = SceneManager:instantiate("prefabs/player_org.prefab.json")
    CreateDamageObj = SceneManager:instantiate("prefabs/player_damage.prefab.json")

    local scene = SceneManager:getCurrentScene()

    playerOrgobj = scene:findEntityByName("OrgPrefab")
    damageObj = scene:findEntityByName("damagePrefab")
    damageTex = damageObj:findComponent("Renderable3DComponent"):getMaterial()
    local component = playerOrgobj:findComponent("Renderable3DComponent")
    normalTex = component:getMaterial()

    transform = self:findTransform()
    body = self:findRigidBody()

    animator = self:findAnimator()
    if animator == nil then return end
    animator:changeAnimation("MainChar2_Idle")

    smokeEmitter = self:findParticleEmitter()

    if smokeEmitter ~= nil then
        smokeEmitter:pause()
        local data = smokeEmitter:getEmitterData()
        defaultEmissionRate = data.emissionRate
        defaultStartSize    = data.startSize
        defaultEndSize      = data.endSize
        defaultSpreadAngle  = data.spreadAngle
    end

    trajectoryLine = TrajectoryLine.new()

    currentHealth = GameManager:getPlayerHealth()
    moveHealthBar()
end

function onMessage(msg)
    if msg.topic == "Catch Me!" then
        projectileSpeed = baseProjSpeed
        hasBall = true
        local scene = SceneManager:getCurrentScene()
        local ballInd = scene:findEntityByName("BallIndicator")
        local uiRender = ballInd:findComponent("UIRenderComponent")
        uiRender:setTransparency(1.0)
    end

    if msg.topic == "Get Hit!" then
        if not invincible then
            flickerCooldown = 2.0
            currentHealth = currentHealth - 1
            GameManager:setPlayerHealth(currentHealth)
            invincible = true
            moveHealthBar()
        end
    end

    if msg.topic == "Here's the Key!" then
        print("Player has the key!")
        hasKey = true
        local scene = SceneManager:getCurrentScene()
        local ballInd = scene:findEntityByName("KeyIndicator")
        local uiRender = ballInd:findComponent("UIRenderComponent")
        uiRender:setTransparency(1.0)
    end

    if trajectoryLine ~= nil then
        trajectoryLine:ballCallback(msg)
    end
end

function onUpdate(dt)

    if kickbackCountdown > 0.0 and not dead then
        kickbackCountdown = kickbackCountdown - dt
        if kickbackCountdown < 0.0 then
            kickbackCountdown = 0.0
        end
    end

    if deathCooldown > 0.0 and dead then
        deathCooldown = deathCooldown - dt
        if deathCooldown <= 0.0 then
            reset()
        end
    end

    scene = SceneManager:getCurrentScene()
    if scene == nil then return end

    ---@type TimeData
    local timeData = Engine.time()

    local projectilesInRange = getProjectilesInRange()

    if Input:isMouseButtonPressed(MouseButton.Right) then
        rightMousePressTime = timeData.unscaledTime
    end

    if targetTimeScale == 1.0 then
        if (timeData.unscaledTime - slowEndTime > slowCooldown)
            and Input:isMouseButtonHeld(MouseButton.Right)
            and (timeData.unscaledTime - rightMousePressTime < slowTimeLeniency)
            and #projectilesInRange > 0 then

            targetTimeScale = slowedTimeScale
            slowStartTime = timeData.unscaledTime
        end
    else
        if #projectilesInRange == 0 or
            Input:isMouseButtonReleased(MouseButton.Right) or
            (timeData.unscaledTime - slowStartTime > slowTimeLimit) then

            targetTimeScale = 1.0
            slowEndTime = timeData.unscaledTime
        end
    end

    isNormalTime = math.abs(timeScale - 1.0) < timeEps
    if math.abs(timeScale - targetTimeScale) > timeEps then
        timeScale = timeScale + (targetTimeScale - timeScale) * timeSlowLerpSpeed * (timeData.unscaledDt)
        Engine.setTimeScale(timeScale)
        dt = timeData.unscaledDt * timeScale
    end

    if cameraTransform == nil or cameraEntity == nil then
        cameraEntity = scene:findEntityByName("Camera")
        if cameraEntity ~= nil then
            cameraTransform = cameraEntity:findTransform()
        end
    end

    -- Audio Part
    playerPos = transform:getWorldPosition()
    Audio.setListenerPosition(playerPos.x,playerPos.y,playerPos.z)
    local camRot = cameraTransform:getWorldRotation()
    local forward = camRot:forward()
    Audio.setListenerForward(forward.x, forward.y, forward.z)
    playerAudio = self:findAudio()
    Audio.setMusicVolume(0.0)

    local emitterPos = Vector3.new(
            playerPos.x,playerPos.y,playerPos.z
            ) -- keep it close for testing
            playerAudio = self:findAudio()
            playerAudio:setPosition(emitterPos)


    if currentHealth <= 0 then
        log("Player has died!")
        if not dead then
            die()
            dead = true
        end
    end

    if invincible and not dead then
        invincibleTimer = invincibleTimer + dt
        flickerTimer = flickerTimer - dt
        flicker()
        if invincibleTimer >= invincibleDuration then
            invincible = false
            invincibleTimer = 0.0
            flickerTimer = 0.1
            local renderComp = self:findComponent("Renderable3DComponent")
            renderComp:setMaterial(normalTex)
        end
    end

    ----

    if not dead then



        -- Sprint
        moveSpeed = Input:isKeyHeld(Key.LShift) and sprintSpeed or baseSpeed

        -- ── Jump ──────────────────────────────────────────────────────
        if Input:isKeyPressed(Key.Space) and isGrounded then
            local vel  = body:getLinearVelocity()
            local mass = body:getBodyData().mass
            body:setLinearVelocity(Vector3.new(vel.x, 0.0, vel.z))
            body:applyImpulse(Vector3.new(0.0, jumpImpulse * mass, 0.0))
            isGrounded = false
        end

        -- ── Jump-cut: release early → shorter hop ─────────────────────
        if Input:isKeyReleased(Key.Space) then
            local vel = body:getLinearVelocity()
            if vel.y > 0.0 then
                body:setLinearVelocity(Vector3.new(vel.x, vel.y * jumpCutMultiplier, vel.z))
            end
        end

        -- Input
        local lStickX = Input:getGamepadAxis(GamepadAxes.LStickX)
        local lStickY = Input:getGamepadAxis(GamepadAxes.LStickY)
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

        local inputMag = math.sqrt(inputX*inputX + inputZ*inputZ)
        if inputMag > 1 then
            inputX = inputX / inputMag
            inputZ = inputZ / inputMag
        end

        local moveDir = getCameraRelativeMovement(inputX, inputZ)

        local vel = body:getLinearVelocity()
        local mass = body:getBodyData().mass

        local desiredX = moveDir.x * moveSpeed
        local desiredZ = moveDir.z * moveSpeed

        local dvx = desiredX - vel.x
        local dvz = desiredZ - vel.z
        local dvMag = math.sqrt(dvx * dvx + dvz * dvz)

        local timeToReach = isGrounded and timeToReachGround or timeToReachAir
        timeToReach = math.max(timeToReach, 0.0001)

        local maxDelta = dvMag * (dt / timeToReach)

        local deltaX, deltaZ = 0.0, 0.0
        if dvMag > dvDeadzone then
            local scale = math.min(1.0, maxDelta / dvMag)
            deltaX = dvx * scale
            deltaZ = dvz * scale
        end

        body:setLinearVelocity(Vector3.new(
            vel.x + deltaX,
            vel.y,
            vel.z + deltaZ
        ))


        if inputMag > 0.01 then
            local animName = animator.currentAnimationName
            if animName ~= "MainChar2_Running"  and kickbackCountdown <= 0.0 and not dead then
                animator:changeAnimation("MainChar2_Running")
                playerAudio:play("footstep")
            end
        else
            local animName = animator.currentAnimationName
            if animName ~= "MainChar2_Idle" and kickbackCountdown <= 0.0 and not dead then
                animator:changeAnimation("MainChar2_Idle")
                playerAudio:stop("footstep")
            end
        end

        if inputMag > 0 then
            local angle = math.atan(moveDir.x, moveDir.z)
            rotateTo(angle, dt)
        end

    end
    playerPos = transform:getWorldPosition()
    playerArmPos = playerPos + spawnHeightVec
    targetPoint = cameraAim(1000.0)
    if targetPoint == nil then return end
    ---@type Vector3
    local shootVector = (targetPoint - playerArmPos)
    if shootVector:Magnitude() == 0.0 then return end
    shootDir = shootVector:Normalized()
    spawnPos = playerArmPos + (shootDir * spawnForward)

    if trajectoryLine ~= nil then
        if not Input:isMouseButtonHeld(MouseButton.Right) then
                trajectoryLine:resetLine()
        else
            if hasBall then
                trajectoryLine:updateTrajectory(spawnPos, shootDir, projectileSpeed, 1.0, ballGravity)
            else
                if not isNormalTime then
                    local closestProjectile = getClosestProjectile(projectilesInRange)
                    if closestProjectile ~= nil then
                        trajectoryLine:updateTrajectory(closestProjectile.position, shootDir, projectileSpeed, 1.0, ballGravity)
                    else
                        trajectoryLine:resetLine()
                    end
                else
                    trajectoryLine:resetLine()
                end
            end
        end
    end

    if Input:isMouseButtonPressed(MouseButton.Left) then
        -- Get the camera aim direction early, so we can reuse it
        if hasBall then
            shootProjectile()
            local ballInd = scene:findEntityByName("BallIndicator")
            local uiRender = ballInd:findComponent("UIRenderComponent")
            uiRender:setTransparency(0.0)
            hasBall = false
            targetTimeScale = 1.0
            slowEndTime = timeData.unscaledTime
        end
        for _, projectile in ipairs(projectilesInRange) do
            kickBack(projectile)
            targetTimeScale = 1.0
            slowEndTime = timeData.unscaledTime
        end
    end
end

function tellCameraMyAnimation(animName)
    local message = Message.new()
    message.to = cameraEntity
    message.topic = "PlayerAnimChange"
    message.contents = animName
    Script.send(message)
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
    if forward:Magnitude() > 0 then forward = forward:Normalized() end
    local right = camRot:right()
    right.y = 0
    if right:Magnitude() > 0 then right = right:Normalized() end
    local moveDir = forward * inputZ + right * inputX
    if moveDir:Magnitude() > 0 then moveDir = moveDir:Normalized() end
    return moveDir
end

function rotateTo(angle, dt)
    local target = Quaternion.fromEuler(0, angle, 0)
    local current = transform:getWorldRotation()
    local rot = Quaternion.slerp(current, target, dt * turnSpeed)
    transform:setWorldRotation(rot)
end


function shootProjectile()
    local proj = SceneManager:instantiate("prefabs/catch_ball.prefab.json")
    if proj == nil then return end
    local projTransform = proj:findTransform()
    local projBody = proj:findGhostBody()
    if projTransform == nil or projBody == nil then
        SceneManager:destroy(proj)
        return
    end
    projTransform:setWorldPosition(spawnPos)
    projBody:setLinearVelocity(shootDir * baseProjSpeed)
end

function onCollisionEnter(data)
    local tagsA = data.entityA:getTags()
    for _, tag in pairs(tagsA) do
        if tag == "ground" then
            if not isGrounded then triggerLandingBurst() end
            isGrounded = true
            jumping = false
        elseif tag == "door" then
            if hasKey then changeLevels() end
        end
    end
    local tagsB = data.entityB:getTags()
    for _, tag in pairs(tagsB) do
        if tag == "ground" then
            if not isGrounded then triggerLandingBurst() end
            isGrounded = true
            jumping = false
        elseif tag == "door" then
            if hasKey then changeLevels() end
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
    local cameraEntity = scene:findEntityByName("Camera")
    if cameraEntity == nil then return end
    local cameraTransform = cameraEntity:findTransform()
    if cameraTransform == nil then return end
    local camRot = cameraTransform:getWorldRotation()
    local aimDir = camRot:forward()
    return aimDir:Normalized()
end

function moveHealthBar()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local healthBar = scene:findEntityByName("HealthBar")
    local barTransform = healthBar:findComponent("UITransformComponent")
    local hSize = barTransform:getSize()
    local maxSize = 160
    local percent = currentHealth / maxHealth
    barTransform:setSize(Vector2.new(maxSize * percent, hSize.y))
end

function changeLevels()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local sceneName = scene:getName()
    if sceneName == "Level1" then SceneManager:changeScene("Level2")
    elseif sceneName == "Level2" then SceneManager:changeScene("Level3")
    elseif sceneName == "Level3" then SceneManager:changeScene("Level4")
    elseif sceneName == "Level4" then SceneManager:changeScene("Level5")
    elseif sceneName == "Level5" then log("Beta Completed!") end
end

function cameraAim(maxDistance)
    local camPos = cameraTransform:getWorldPosition()
    local forward = cameraTransform:getWorldRotation():forward():Normalized()

    local hit = Physics.Raycast(camPos, forward, maxDistance, { layerMask = Layers.World | Layers.Enemy })
    if hit ~= nil then return hit.position end
    return camPos + forward * maxDistance
end

--- Kickback projectile
---@param projectile Projectile Projectile being kicked back
function kickBack(projectile)
    ---@type Entity
    local entity = projectile.entity
    local projBody = entity:findGhostBody()
    if projBody ~= nil then
        local kickDir = (targetPoint - projectile.position)
        if kickDir:Magnitude() > 0.0001 then
            if animName ~= "Kicking" then
                animator:changeAnimation("Kicking")
                kickbackCountdown = 0.7
            end
            kickDir = kickDir:Normalized()
            projBody:setLinearVelocity(kickDir * baseProjSpeed)
            local message = Message.new()
            message.to = entity
            message.topic = "I am kicking you!"
            Script.send(message)

            local kickbackEmitter = scene:findEntityByName("KickbackEmitter")
            if kickbackEmitter ~= nil then
                local kickbackPE = kickbackEmitter:findParticleEmitter()
                kickbackPE:playFor(0.1)
            end 
        end
    end
end


function die()
    if animName ~= "Death" then
        animator:changeAnimation("Death")
    end
    deathCooldown = 2.5
end


function reset()
    currentHealth = maxHealth
    GameManager:setPlayerHealth(maxHealth)
    Window.setCursorMode(0, Window.CursorMode.Normal)
    SceneManager:changeScene("MainMenu")
end

function flicker()
    local renderComp = self:findComponent("Renderable3DComponent")
    if flickerTimer < 0.0 then
        if renderComp:getMaterial() == normalTex
        then
                renderComp:setMaterial(damageTex)
            else
                renderComp:setMaterial(normalTex)
        end
        flickerTimer = 0.1
    end
end
