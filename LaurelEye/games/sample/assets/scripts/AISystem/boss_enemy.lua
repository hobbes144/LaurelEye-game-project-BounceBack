-- Boss Enemy AI (Phase-based, message-driven)

local StateMachine = require("StateMachine")
local State = require("State")

-----------------------------------
-- Variables
-----------------------------------

local transform = nil
local body = nil
local animator = nil
local player = nil
local destroyed = false
local shield = nil

-- Phase system
local phase = 1
local generators_remaining = 1

-- Movement
local turn_speed = 6.0
local move_speed    = 8.0
local orbit_radius = 12.0
local orbit_speed = 20.0
local orbit_angle = 0.0
local generator = nil

-- Shooting
local shot_timer = 0.0
local shoot_delay = 2.5

-- Stun
local stunned = false
local stun_timer = 0.0

-----------------------------------
-- State machine
-----------------------------------

local state_machine = nil

local idle_state = nil
local startle_state = nil
local orbit_state = nil
local shoot_state = nil
local stunned_state = nil
local phase3_state = nil
local dead_state = nil
local active = false
-----------------------------------

soundFactor = 50

function onStart()
    transform = self:findTransform()
    body = self:findRigidBody()

    animator = self:findAnimator()
    if animator == nil then return end
    animator:changeAnimation("Boss_armor_Idle")

    shield = SceneManager:instantiate("prefabs/shield.prefab.json")
    local message  = Message.new()
    message.to     = shield
    message.from   = self
    message.topic  = "Here I Am!"
    Script.send(message)

    local scene = SceneManager:getCurrentScene()
    generator = scene:findEntityByName("FinalGenerator")

    setupStateMachine()
end

function onUpdate(dt)
    if state_machine then
        state_machine:update(dt)
    end
end

-----------------------------------
-- Message Handling (CORE)
-----------------------------------

function onMessage(msg)

    if msg.topic == "Get Hit!" then
        local enemyAudio = self:findAudio()
        enemyAudio:stop("Hitting")
        local BallPos = transform:getWorldPosition()
        local emitterPos = Vector3.new(
            BallPos.x/soundFactor,BallPos.y/soundFactor,BallPos.z/soundFactor
        )
        enemyAudio:setPosition(emitterPos)
        enemyAudio:play("Hitting")
        stunBoss(phase == 1 and 2.0 or 1.0)
        state_machine:forceTransition("Stunned")
        return
    end

    if msg.topic == "I Have Been Destroyed!" then
        generators_remaining = generators_remaining - 1

        print("Generators Left: ", generators_remaining)

        if generators_remaining <= 0 and phase == 1 then
            enterPhase2()
        end
        return
    end

    if msg.topic == "Avenge Me!" then
        enterPhase3()
        return
    end

    if msg.topic == "We Bonked!" then      -- ball hit the shield
        stunBoss(phase == 1 and 2.0 or 1.0)
        state_machine:forceTransition("Stunned")
        return true
    end
end

-----------------------------------
-- Phase Control
-----------------------------------

function enterPhase2()
    generator = SceneManager:instantiate("prefabs/final_generator.prefab.json")
    phase = 2
    shoot_delay = 1.5
    log("Boss Phase 2")
end

function enterPhase3()
    SceneManager:destroy(shield)
    local scene = SceneManager:getCurrentScene()
    local newBoss = scene:findEntityByName("BossEnemy2")
    local pos = transform:getWorldPosition()
    local newTransform = newBoss:findTransform()
    newTransform:setWorldPosition(pos)

    local message  = Message.new()
    message.to     = newBoss
    message.topic  = "Get Fucking Going"
    Script.send(message)

    SceneManager:destroy(self)
end

function stunBoss(duration)
    stunned = true
    stun_timer = duration
end

-----------------------------------
-- State Machine
-----------------------------------

function setupStateMachine()
    state_machine = StateMachine.new()

    idle_state = State.new("Idle")
    startle_state = State.new("Startle")
    orbit_state = State.new("Orbit")
    shoot_state = State.new("Shoot")
    stunned_state = State.new("Stunned")

    setupIdleState()
    setupStartleState()
    setupOrbitState()
    setupShootState()
    setupStunnedState()

    state_machine:addState(idle_state)
    state_machine:addState(startle_state)
    state_machine:addState(orbit_state)
    state_machine:addState(shoot_state)
    state_machine:addState(stunned_state)

    state_machine:setInitialState("Idle")
end

-----------------------------------
-- Idle
-----------------------------------

function setupIdleState()
    idle_state:setOnEnter(function()
        log("Boss idle")
    end)

    idle_state:setUpdate(function(dt)
        player = getPlayer()
        if player ~= nil then
            active = true
        end
    end)

    idle_state:addTransition("toStartle", startle_state, 1, function()
        return active
    end)
end

-----------------------------------
-- Startle
-----------------------------------

function setupStartleState()
    local timer = 0

    startle_state:setOnEnter(function()
        timer = 1.0
        animator:changeAnimation("alert")
    end)

    startle_state:setUpdate(function(dt)
        timer = timer - dt
    end)

    startle_state:addTransition("toOrbit", orbit_state, 1, function()
        return timer <= 0
    end)
end

-----------------------------------
-- Orbit
-----------------------------------

function setupOrbitState()
    orbit_state:setOnEnter(function()
        log("Boss orbit / phase behavior")
        shot_timer = 0.0
        animator:changeAnimation("Running")
    end)

    orbit_state:setUpdate(function(dt)
        if stunned then return end

        rotateTowardsPlayer(dt)

        -- PHASE-BASED MOVEMENT
        if phase == 1 or phase == 3 then
            moveTowardsPlayer(dt)
        elseif phase == 2 then
            Phase2Movement(dt)
        end

        shot_timer = shot_timer + dt
    end)

    orbit_state:addTransition("toShoot", shoot_state, 1, function()
        return shot_timer >= shoot_delay
    end)
end

-----------------------------------
-- Shoot
-----------------------------------

function setupShootState()

    local state_duration
    local shot = false

    shoot_state:setOnEnter(function()
        log("Boss shooting")
        animator:changeAnimation("RunThrowGrenade")
        state_duration = 1.0

        shot = false
    end)

    shoot_state:setUpdate(function(dt)
        if stunned then return end

        state_duration = state_duration - dt

        rotateTowardsPlayer(dt)

        if phase == 2 then
            Phase2Movement(dt)
        end

        if state_duration < 0.5 then
            if not shot then
                autoShootProjectile(dt)
                shot = true
            end
        end
    end)

    shoot_state:addTransition("toOrbit", orbit_state, 1, function()
        return state_duration <= 0.0
    end)
end

-----------------------------------
-- Stunned
-----------------------------------

function setupStunnedState()
    stunned_state:setOnEnter(function()
        log("Boss stunned")
    end)

    stunned_state:setUpdate(function(dt)
        stun_timer = stun_timer - dt
        if stun_timer <= 0 then
            stunned = false
        end
    end)

    stunned_state:addTransition("toOrbit", orbit_state, 1, function()
        return not stunned
    end)
end

-----------------------------------
-- Helpers
-----------------------------------

function Phase2Movement(dt)
    if generator == nil or player == nil or body == nil then return end

    local genTransform = generator:findTransform()
    local playerTransform = player:findTransform()

    local genPos    = genTransform:getWorldPosition()
    local playerPos = playerTransform:getWorldPosition()

    local dir = playerPos - genPos
    dir.y = 0
    if dir:Magnitude() == 0 then return end
    dir = dir:Normalized()

    local targetPos = genPos + dir * orbit_radius

    local selfPos = transform:getWorldPosition()
    local moveVec = targetPos - selfPos
    moveVec.y = 0

    local distance = moveVec:Magnitude()

    if distance < 1.0 then
        local vel = body:getLinearVelocity()
        vel.y = body:getLinearVelocity().y
        body:setLinearVelocity(vel)
        return
    end

    local moveDir = moveVec:Normalized()

    -- Scale speed with distance so he catches up quickly when far behind
    local speed = math.min(orbit_speed * (distance / orbit_radius), orbit_speed * 10.0)
    speed = math.max(speed, orbit_speed)

    local vel = moveDir * speed
    vel.y = body:getLinearVelocity().y
    body:setLinearVelocity(vel)
end

function rotateTowardsPlayer(dt)
    local playerTransform = player:findTransform()
    if playerTransform == nil then return end

    local selfPos = transform:getWorldPosition()
    local playerPos = playerTransform:getWorldPosition()

    local dir = playerPos - selfPos
    dir.y = 0

    if dir:Magnitude() == 0 then return end
    dir = dir:Normalized()

    local angle = math.atan(-dir.x, -dir.z) + math.pi
    local target = Quaternion.fromEuler(0, angle, 0)

    local current = transform:getWorldRotation()
    local rot = Quaternion.slerp(current, target, dt * turn_speed)

    transform:setWorldRotation(rot)
end

-----------------------------------
-- Shooting
-----------------------------------

function autoShootProjectile(dt)
    if self == nil then return end
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end

    -- Find enemies by tag
    local player = scene:findEntityByName("PlayerPrefab")
    if player == nil then return end

    -- Player transform + spawn offset
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

    -- Find player
    local playerTransform = player:findTransform()
    if playerTransform == nil then end

    local playerPos = playerTransform:getWorldPosition()

    if playerPos == nil then return end
    playerPos.y = spawnPos.y

    -- Instantiate projectile and aim at the chosen target
    local proj = SceneManager:instantiate("prefabs/projectile.prefab.json")
    if proj == nil then return end

    local projTransform = proj:findTransform()
    if projTransform == nil then return end

    projTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local dir = playerPos - spawnPos
    if dir:Magnitude() > 0 then
        dir = dir:Normalized()
    else
        dir = forward -- fallback
    end

    local arcAngle = 0.05
    dir.y = math.tan(arcAngle)
    if dir:Magnitude() > 0 then
        dir = dir:Normalized()
    end

    local projBody = proj:findGhostBody()
    if projBody ~= nil then
        local projectileSpeed = 75.0 -- tune as needed
        projBody:setLinearVelocity(dir * projectileSpeed)
    end
end

-----------------------------------
-- Movement helpers (ADD THIS)
-----------------------------------

function moveTowardsPlayer(dt)
    local player = getPlayer()
    if player == nil then return end

    local player_transform = player:findTransform()
    if player_transform == nil then return end
    if transform == nil then return end

    local self_pos   = transform:getWorldPosition()
    local player_pos = player_transform:getWorldPosition()
    local dir        = player_pos - self_pos
    dir.y = 0

    if dir:Magnitude() == 0 then return end
    dir = dir:Normalized()

    if body ~= nil then
        local vel = dir * move_speed
        vel.y = body:getLinearVelocity().y
        body:setLinearVelocity(vel)
    else
        local new_pos = self_pos + dir * move_speed * dt
        transform:setWorldPosition(new_pos.x, new_pos.y, new_pos.z)
    end
end

-----------------------------------

function getPlayer()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return nil end
    return scene:findEntityByName("PlayerPrefab")
end

