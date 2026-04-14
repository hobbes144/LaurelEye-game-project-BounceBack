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

local health = 5
local start = false

local turn_speed = 6.0
local move_speed    = 8.0
local ballSpawned = false

-- Shooting
local shot_timer = 0.0
local shoot_delay = 0.5

-- Stun
local stunned = false
local stun_timer = 0.0

-----------------------------------
-- State machine
-----------------------------------

local state_machine = nil

local idle_state = nil
local orbit_state = nil
local shoot_state = nil
local stunned_state = nil
local dead_state = nil
local active = false
-----------------------------------

function onStart()
    transform = self:findTransform()
    body = self:findRigidBody()

    animator = self:findAnimator()
    if animator == nil then return end
    animator:changeAnimation("Boss_Running")

    setupStateMachine()
end

function onUpdate(dt)
    if state_machine and start then
        state_machine:update(dt)
    end
end

-----------------------------------
-- Message Handling (CORE)
-----------------------------------

function onMessage(msg)
    if msg.topic == "Get Hit!" then
        log("Security hit by ball!")
        health = health - 1
        if health <= 0 then
            state_machine:forceTransition("Dead")
        else
            stunBoss(1.0)
            state_machine:forceTransition("Stunned")
        end
        return true
    end

    if msg.topic == "Get Fucking Going" then
        player = getPlayer()
        start = true
    end
end

-----------------------------------
-- Phase Control
-----------------------------------

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
    orbit_state = State.new("Orbit")
    shoot_state = State.new("Shoot")
    stunned_state = State.new("Stunned")
    dead_state = State.new("Dead")

    setupIdleState()
    setupOrbitState()
    setupShootState()
    setupStunnedState()
    setupDeadState()

    state_machine:addState(idle_state)
    state_machine:addState(orbit_state)
    state_machine:addState(shoot_state)
    state_machine:addState(stunned_state)
    state_machine:addState(dead_state)

    state_machine:setInitialState("Orbit")
end

-----------------------------------
-- Idle
-----------------------------------

function setupIdleState()
    idle_state:setOnEnter(function()
        log("Boss idle")
    end)

    idle_state:setUpdate(function(dt)
        if player ~= nil then
            active = true
        end
    end)

    idle_state:addTransition("toOrbit", orbit_state, 1, function()
        return active
    end)
end

-----------------------------------
-- Orbit
-----------------------------------

function setupOrbitState()
    orbit_state:setOnEnter(function()
        log("Boss orbit / phase behavior")
        shot_timer = 0.0
        animator:changeAnimation("Boss_Running")
    end)

    orbit_state:setUpdate(function(dt)
        if stunned then return end

        rotateTowardsPlayer(dt)
        moveTowardsPlayer(dt)

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
        animator:changeAnimation("BossThrow")
        state_duration = 1.0

        shot = false
    end)

    shoot_state:setUpdate(function(dt)
        if stunned then return end
        state_duration = state_duration - dt

        rotateTowardsPlayer(dt)

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
        animator:changeAnimation("BossGettingHit")
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
-- Dead
-----------------------------------

function setupDeadState()
    local state_duration

    dead_state:setOnEnter(function()
        state_duration = 3.0
        if animator then animator:changeAnimation("BossDeath") end
    end)

    dead_state:setUpdate(function(dt)
        state_duration = state_duration - dt
        if state_duration <= 0 then
            animator:setTime(2.9)
            if not ballSpawned then
                local explosive = SceneManager:instantiate("prefabs/explosive_ball.prefab.json")
                local expTransform = explosive:findTransform()
                local pos = transform:getWorldPosition()
                expTransform:setWorldPosition(pos.x, 1.0, pos.z - 10.0)
                ballSpawned = true
            end
        end
    end)
end

-----------------------------------
-- Helpers
-----------------------------------

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

