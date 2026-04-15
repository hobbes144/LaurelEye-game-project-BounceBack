-- Security Enemy AI
-- Takes 3 hits to destroy, moves toward player instead of shooting

local StateMachine = require("StateMachine")
local State = require("State")

-----------------------------------
-- Enemy variables
-----------------------------------

local turn_speed    = 5.0
local move_speed    = 8.0
local attack_distance = 8.0
local transform     = nil
local body          = nil
local destroyed     = false
local hit_count     = 0
local max_hits      = 3
local animator      = nil
local shield        = nil
local hasKey        = false

-----------------------------------
-- State machine & states
-----------------------------------

local enemy_ai      = nil

local idle_state      = nil
local alert_state     = nil
local moving_state    = nil
local attacking_state = nil
local get_hit_state   = nil
local dead_state      = nil
local take_damage_state = nil

soundFactor = 50
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------

function onStart()
    transform = self:findTransform()
    body      = self:findRigidBody()

    animator = self:findAnimator()
    if animator == nil then return end
    animator:changeAnimation("Security_Idle")

    shield = SceneManager:instantiate("prefabs/shield.prefab.json")
    local message  = Message.new()
    message.to     = shield
    message.from   = self
    message.topic  = "Here I Am!"
    Script.send(message)

    setupStateMachine()
end

function onMessage(msg)
    if msg.topic == "Get Hit!" then
        log("Security hit by ball!")
        local enemyAudio = self:findAudio()
        enemyAudio:stop("Hitting")
        local BallPos = transform:getWorldPosition()
        local emitterPos = Vector3.new(
            BallPos.x/soundFactor,BallPos.y/soundFactor,BallPos.z/soundFactor
        )
        enemyAudio:setPosition(emitterPos)
        enemyAudio:play("Hitting")
        hit_count = hit_count + 1
        if hit_count >= max_hits then
            if enemy_ai then enemy_ai:forceTransition("Dead") end
        else
            if enemy_ai then enemy_ai:forceTransition("TakeDamage") end
        end
        return true
    end
    if msg.topic == "We Bonked!" then      -- ball hit the shield
        if enemy_ai then enemy_ai:forceTransition("GetHit") end
        return true
    end
    if msg.topic == "I have the key!" then
        print("Enemy has been given the key")
        hasKey = true
    end
end

function onUpdate(dt)
    if enemy_ai then
        enemy_ai:update(dt)
    end
end

---------------------------------------------------------------------------------------
-- State machine setup

function setupStateMachine()
    enemy_ai = StateMachine.new()

    idle_state      = State.new("Idle")
    alert_state     = State.new("Alert")
    moving_state    = State.new("Moving")
    attacking_state = State.new("Attacking")
    get_hit_state   = State.new("GetHit")
    take_damage_state = State.new("TakeDamage")
    dead_state      = State.new("Dead")

    setupIdleState()
    setupAlertState()
    setupMovingState()
    setupAttackingState()
    setupGetHitState()
    setupTakeDamageState()
    setupDeadState()

    enemy_ai:addState(idle_state)
    enemy_ai:addState(alert_state)
    enemy_ai:addState(moving_state)
    enemy_ai:addState(attacking_state)
    enemy_ai:addState(get_hit_state)
    enemy_ai:addState(take_damage_state)
    enemy_ai:addState(dead_state)

    enemy_ai:setInitialState("Idle")
end

-----------------------------------
-- Idle state
-----------------------------------

function setupIdleState()
    idle_state:setOnEnter(function()
        log("Security: idle")
        if animator then animator:changeAnimation("Security_Idle") end
    end)

    idle_state:setUpdate(function(dt)
        -- Just wait
    end)

    idle_state:addTransition("toAlert", alert_state, 1, function()
        return getPlayer() ~= nil
    end)
end

-----------------------------------
-- Alert state
-----------------------------------

function setupAlertState()
    local alert_timer = 0.0

    alert_state:setOnEnter(function()
        log("Security: alert! Player spotted")
        alert_timer = 0.0
        if animator then animator:changeAnimation("Security_Idle") end
    end)

    alert_state:setUpdate(function(dt)
        alert_timer = alert_timer + dt
        rotateTowardsPlayer(dt)
    end)

    alert_state:addTransition("toMoving", moving_state, 1, function()
        return alert_timer >= 1.5
    end)
end

-----------------------------------
-- Moving state
-----------------------------------

function setupMovingState()
    moving_state:setOnEnter(function()
        log("Security: moving towards player")
        if animator then animator:changeAnimation("Running") end
    end)

    moving_state:setUpdate(function(dt)
        rotateTowardsPlayer(dt)
        moveTowardsPlayer(dt)
    end)

    moving_state:addTransition("toAttacking", attacking_state, 1, function()
        return isPlayerInRange(attack_distance)
    end)
end

-----------------------------------
-- Attacking state
-----------------------------------

function setupAttackingState()
    attacking_state:setOnEnter(function()
        log("Security: attacking with shield!")
        if animator then animator:changeAnimation("Security_Idle") end
        local message  = Message.new()
        message.to     = shield
        message.topic  = "Attack!"
        Script.send(message)
    end)

    attacking_state:setUpdate(function(dt)
        rotateTowardsPlayer(dt)
    end)

    attacking_state:addTransition("toMoving", moving_state, 1, function()
        return not isPlayerInRange(attack_distance)
    end)
end

-----------------------------------
-- GetHit state
-----------------------------------

function setupGetHitState()
    local hit_timer = 0.0

    get_hit_state:setOnEnter(function()
        log("Security: got hit! Hits taken: " .. hit_count)
        hit_timer = 0.0
        if animator then animator:changeAnimation("Security_Idle") end
    end)

    get_hit_state:setUpdate(function(dt)
        hit_timer = hit_timer + dt
    end)

    get_hit_state:addTransition("toMoving", moving_state, 1, function()
        return hit_timer >= 0.5
    end)
end

-----------------------------------
-- TakeDamage state
-----------------------------------
function setupTakeDamageState()
    local hit_timer = 0.0

    take_damage_state:setOnEnter(function()
        log("Security: got hit! Hits taken: " .. hit_count)
        hit_timer = 0.0
        if animator then animator:changeAnimation("Hit") end
    end)

    take_damage_state:setUpdate(function(dt)
        hit_timer = hit_timer + dt
    end)

    take_damage_state:addTransition("toMoving", moving_state, 1, function()
        return hit_timer >= 1.0
    end)
end


-----------------------------------
-- Dead state
-----------------------------------

function setupDeadState()

    local state_duration

    dead_state:setOnEnter(function()
        state_duration = 2.0
        log("Security: destroyed after " .. max_hits .. " hits")
        local message  = Message.new()
        message.to     = shield
        message.topic  = "We Die Together!"
        Script.send(message)
        if animator then animator:changeAnimation("Death") end
    end)

    dead_state:setUpdate(function(dt)
        state_duration = state_duration - dt
        if state_duration <= 0 then
            destroySelf()
        end
    end)
end

---------------------------------------------------------------------------------------
-- Helper functions
---------------------------------------------------------------------------------------

function onCollisionEnter(data)
    local function checkTags(entity)
        local tags = entity:getTags()
        for _, tag in pairs(tags) do
            if tag == "ball" then
                log("Security hit by ball!")
                hit_count = hit_count + 1
                if hit_count >= max_hits then
                    if state_machine.current_state.name == "Dead" then return end
                    if enemy_ai then enemy_ai:forceTransition("Dead") end
                else
                    if enemy_ai then enemy_ai:forceTransition("TakeDamage") end
                end
                return true
            end
        end
        return false
    end

    if not checkTags(data.entityA) then
        checkTags(data.entityB)
    end
end

function onCollisionStay(data) end
function onCollisionExit(data) end

function getPlayer()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return nil end
    return scene:findEntityByName("PlayerPrefab")
end

function isPlayerInRange(range)
    local player = getPlayer()
    if player == nil then return false end

    local player_transform = player:findTransform()
    if player_transform == nil then return false end
    if transform == nil then return false end

    local self_pos   = transform:getWorldPosition()
    local player_pos = player_transform:getWorldPosition()
    local distance   = (player_pos - self_pos):Magnitude()

    return distance < range
end

function rotateTowardsPlayer(dt)
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

    local front_offset = math.pi
    local angle        = math.atan(-dir.x, -dir.z) + front_offset
    local target_quat  = Quaternion.fromEuler(0, angle, 0)
    local current_rot  = transform:getWorldRotation()
    local rot          = Quaternion.slerp(current_rot, target_quat, dt * turn_speed)

    transform:setWorldRotation(rot)
end

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

function destroySelf()
    if destroyed then return end
    destroyed = true
    if hasKey then
        local key = SceneManager:instantiate("prefabs/keycard.prefab.json")
        local keyTransform = key:findTransform()
        local pos = transform:getWorldPosition()
        keyTransform:setWorldPosition(pos.x, 3.0, pos.z)
    end
    GameManager:addEnemyKilled(1)
    SceneManager:destroy(self)
end
