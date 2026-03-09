--Claude example of an enemy that uses the state machine
--This should have identical behavior to the static initial ai in the game


local StateMachine = require("StateMachine")
local State = require("State")

-- Enemy data
turnSpeed = 8.0
transform = nil
body = nil
destroyed = false
shotTimer = 0.0
shootingSpeed = 3.0
minShootingSpeed = 2.5
maxShootingSpeed = 4.0

-- State machine
enemyAI = nil

function onStart()
    transform = self:findTransform()
    body = self:findRigidBody()
    shootingSpeed = math.random(minShootingSpeed * 100, maxShootingSpeed * 100) / 100

    -- Setup state machine
    setupStateMachine()
end

function setupStateMachine()
    enemyAI = StateMachine.new()

    -- Create states
    local idle = State.new("Idle")
    local tracking = State.new("Tracking")
    local shooting = State.new("Shooting")
    local dead = State.new("Dead")

    -- IDLE STATE
    idle:setOnEnter(function()
        log("Enemy idle")
    end)

    idle:setUpdate(function(dt)
        -- Just wait
    end)

    idle:addTransition("toTracking", tracking, 1, function()
        return getPlayer() ~= nil  -- Start tracking when player exists
    end)

    -- TRACKING STATE (rotate toward player)
    tracking:setOnEnter(function()
        log("Enemy tracking player")
    end)

    tracking:setUpdate(function(dt)
        rotateTowardsPlayer(dt)
    end)

    tracking:addTransition("toShooting", shooting, 1, function()
        return isPlayerInRange(30.0)  -- Close enough to shoot
    end)

    tracking:addTransition("toIdle", idle, 2, function()
        return getPlayer() == nil  -- Lost player
    end)

    -- SHOOTING STATE (rotate + shoot)
    shooting:setOnEnter(function()
        log("Enemy shooting!")
        shotTimer = 0.0
    end)

    shooting:setUpdate(function(dt)
        rotateTowardsPlayer(dt)
        autoShootProjectile(dt)
    end)

    shooting:addTransition("toTracking", tracking, 1, function()
        return not isPlayerInRange(30.0)  -- Player too far
    end)


    shooting:addTransition("toIdle", idle, 2, function()
        return getPlayer() == nil  -- Lost player
    end)

    -- DEAD STATE
    dead:setOnEnter(function()
        log("Enemy destroyed")
        destroySelf()
    end)

    dead:setUpdate(function(dt)
        -- Do nothing, already dead
    end)

    -- Add states to machine
    enemyAI:addState(idle)
    enemyAI:addState(tracking)
    enemyAI:addState(shooting)
    enemyAI:addState(dead)

    -- Set initial state
    enemyAI:setInitialState("Idle")
end

function onUpdate(dt)
    if enemyAI then
        enemyAI:update(dt)
    end
end

function onCollisionEnter(data)
    local tagsA = data.entityA:getTags()
    for _, tag in pairs(tagsA) do
        if tag == "ball" then
            log("Collided with Ball!")
            -- Force transition to dead state
            if enemyAI then
                enemyAI:forceTransition("Dead")
            end
        end
    end

    local tagsB = data.entityB:getTags()
    for _, tag in pairs(tagsB) do
        if tag == "ball" then
            log("Collided with Ball!")
            if enemyAI then
                enemyAI:forceTransition("Dead")
            end
        end
    end
end

function onCollisionStay(data) end
function onCollisionExit(data) end

-- Helper functions
function getPlayer()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return nil end
    return scene:findEntityByName("PlayerPrefab")
end

function isPlayerInRange(range)
    local player = getPlayer()
    if player == nil then return false end

    local playerTransform = player:findTransform()
    if playerTransform == nil then return false end
    if transform == nil then return false end

    local selfPos = transform:getWorldPosition()
    local playerPos = playerTransform:getWorldPosition()
    local distance = (playerPos - selfPos):Magnitude()

    return distance < range
end

function rotateTowardsPlayer(dt)
    local player = getPlayer()
    if player == nil then return end

    local playerTransform = player:findTransform()
    if playerTransform == nil then return end
    if transform == nil then return end

    local selfPos = transform:getWorldPosition()
    local playerPos = playerTransform:getWorldPosition()
    local dir = playerPos - selfPos
    dir.y = 0

    if dir:Magnitude() == 0 then return end
    dir = dir:Normalized()

    local frontOffset = math.pi
    local angle = math.atan(-dir.x, -dir.z) + frontOffset
    local targetQuat = Quaternion.fromEuler(0, angle, 0)
    local currentRot = transform:getWorldRotation()
    local rot = Quaternion.slerp(currentRot, targetQuat, dt * turnSpeed)

    transform:setWorldRotation(rot)
end

function autoShootProjectile(dt)
    if dt ~= nil then
        shotTimer = shotTimer + dt
        if shotTimer < shootingSpeed then return end
        shotTimer = shotTimer - shootingSpeed
    end

    if self == nil then return end

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end

    local player = getPlayer()
    if player == nil then return end

    local selfTransform = self:findTransform()
    if selfTransform == nil then return end

    local pos = selfTransform:getWorldPosition()
    local selfRotation = selfTransform:getWorldRotation()
    local forward = selfRotation:forward()
    forward.y = 0

    if forward:Magnitude() > 0 then
        forward = forward:Normalized()
    end

    local spawnDistance = -3.5
    local verticalOffset = 8.0
    local spawnPos = pos + forward * spawnDistance
    spawnPos.y = spawnPos.y + verticalOffset

    local playerTransform = player:findTransform()
    if playerTransform == nil then return end

    local playerPos = playerTransform:getWorldPosition()
    if playerPos == nil then return end
    playerPos.y = spawnPos.y

    local proj = SceneManager:instantiate("prefabs/projectile.prefab.json")
    if proj == nil then return end

    local projTransform = proj:findTransform()
    if projTransform == nil then return end
    projTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local dir = playerPos - spawnPos
    if dir:Magnitude() > 0 then
        dir = dir:Normalized()
    else
        dir = forward
    end

    local projBody = proj:findRigidBody()
    if projBody ~= nil then
        local projectileSpeed = 100.0
        projBody:setLinearVelocity(dir * projectileSpeed)
    end
end

function destroySelf()
    if destroyed then return end
    destroyed = true
    SceneManager:destroy(self)
end
