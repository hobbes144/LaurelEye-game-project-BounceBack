

local StateMachine = require("StateMachine")
local State = require("State")

-----------------------------------
--Enemy variables
-----------------------------------

local turn_speed = 8.0
local transform = nil
local body = nil
--local destroyed = false
local shot_timer = 10.0
local shooting_speed = 3.0 -- Default value
local min_shooting_speed = 2.5
local max_shooting_speed = 4.0
local player = nil
local player_transform = nil
--local death_triggered = false
local hasKey = false
local animator = nil
local shot = false
local player_transform = nil

local active = false

-----------------------------------
--State machine & states
-----------------------------------

local state_machine = nil

local idle_state = nil
local startle_state = nil
local alert_state = nil
local shoot_state = nil
local dead_state = nil

---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------



function onStart()
    transform = self:findTransform()
    body = self:findRigidBody()
    --shooting_speed = math.random(minshooting_speed * 100, maxshooting_speed * 100) / 100
    local scene = SceneManager:getCurrentScene()
    player = scene:findEntityByName("PlayerPrefab")

    animator = self:findAnimator()

    print(tostring(self) .. " active=" .. tostring(active))
    active = false  -- does this reset it for all enemies?

    if animator == nil then return end
    animator:changeAnimation("Idle")
    setupStateMachine()

end

function onMessage(msg)
    if msg.topic == "I have the key!" then
        print("Enemy has been given the key")
        hasKey = true
    end
    if msg.topic == "Get Hit!" then
        if msg.to ~= self then return end 
        if hasKey then
            local scene = SceneManager:getCurrentScene()
            local player = scene:findEntityByName("PlayerPrefab")
            local message = Message.new()
            message.to = player
            message.topic = "Here's the Key!"
            Script.send(message)
        end
        if state_machine.current_state.name == "Dead" then return end
        if state_machine then state_machine:forceTransition("Dead") end
    end

end

function onUpdate(dt)
    if state_machine then
        state_machine:update(dt)
    end
end

---------------------------------------------------------------------------------------
--State machine setup

function setupStateMachine()

    state_machine = StateMachine.new()

    idle_state = State.new("Idle")
    startle_state = State.new("Startle")
    alert_state = State.new("Alert")
    shoot_state = State.new("Shoot")
    dead_state = State.new("Dead")

    setupIdleState()
    setupStartleState()
    setupAlertState()
    setupShootState()
    setupDeadState()

    state_machine:addState(idle_state)
    state_machine:addState(startle_state)
    state_machine:addState(alert_state)
    state_machine:addState(shoot_state)
    state_machine:addState(dead_state)

    state_machine:setInitialState("Idle")

end

-----------------------------------
--Idle state
-----------------------------------

function setupIdleState()
    idle_state:setOnEnter(function()
        animator:changeAnimation("Idle")
        log("Enemy idle")
    end)
    
    idle_state:setUpdate(function(dt)
        --log("Updating idle state")
        ballCheck()
    end)
    
    idle_state:addTransition("toStartle", startle_state, 1, function()
        return active
    end)

end

-----------------------------------
--Startle state
-----------------------------------

function setupStartleState()

    local state_duration

    startle_state:setOnEnter(function()
        animator:changeAnimation("Surprised")
        player = getPlayer()
        if player == nil then return end
        state_duration = 1.0
    end)
    
    startle_state:setUpdate(function(dt)
        -- You can add logic for the startle state here (e.g., a brief pause or animation)
        state_duration = state_duration - dt
        rotateTowardsPlayer(dt)
    end)
    
    startle_state:addTransition("toAlert", alert_state, 1, function()
        return state_duration <= 0.0  -- Transition to alert state after startle is done
    end)

end
    
-----------------------------------
--Alert state
-----------------------------------

function setupAlertState()

    local alert_timer = 0.0
    local alert_duration = 2.0

    alert_state:setOnEnter(function()
        log("Enemy alert and tracking player")
        alert_duration = math.random(300, 500) / 100.0  -- random between 1.5s and 3.5s
        alert_timer = 0.0
        animator:changeAnimation("Alert")
    end)
    
    alert_state:setUpdate(function(dt)
        rotateTowardsPlayer(dt)
        alert_timer = alert_timer + dt
        -- You can add logic for the alert state here (e.g., rotate towards player)
    end)
    
    alert_state:addTransition("toShoot", shoot_state, 1, function()
        --after a delay of a random number of seconds - to add variety
        return alert_timer >= alert_duration
    end)

end

-----------------------------------
--Attack state
-----------------------------------

function setupShootState()

    local state_duration

    shoot_state:setOnEnter(function()
        log("Enemy shooting at player")
        animator:changeAnimation("Throw")
        state_duration = 1.0
        shot = false
    end)
    
    shoot_state:setUpdate(function(dt)
        state_duration = state_duration - dt
        if state_duration < 0.5 then
            if not shot then
                rotateTowardsPlayer(dt)
                autoShootProjectile(dt)
                shot = true
            end
        end
    end)
    
    shoot_state:addTransition("toAlert", alert_state, 1, function()
        return state_duration <= 0.0 
    end)
end

-----------------------------------
--Dead state
-----------------------------------

function setupDeadState()

    local state_duration
    local death_triggered = false  -- now local to this closure
    local destroyed = false        -- now local to this closure

    dead_state:setOnEnter(function()
        log("Enemy destroyed")
        death_triggered  = true
        animator:changeAnimation("Death")
        state_duration = 2.0
    end)

    dead_state:setUpdate(function(dt)
        state_duration = state_duration - dt
        if state_duration <= 0 then
            destroySelf()
        end
    end)
end

---------------------------------------------------------------------------------------
--Helper Functions
---------------------------------------------------------------------------------------

function onCollisionStay(data) end
function onCollisionExit(data) end

function onTriggerEnter(data)
    if state_machine.current_state.name == "Dead" then return end  -- already dying

    local function handleBallHit()
        log("Collided with Ball!")
        if hasKey then
            local message = Message.new()
            message.to = player
            message.topic = "Here's the Key!"
            Script.send(message)
        end
        if state_machine then state_machine:forceTransition("Dead") end
    end

    local tagsA = data.entityA:getTags()
    for _, tag in pairs(tagsA) do
        if tag == "ball" then handleBallHit() return end
    end

    local tagsB = data.entityB:getTags()
    for _, tag in pairs(tagsB) do
        if tag == "ball" then handleBallHit() return end
    end
end

function getPlayer()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return nil end
    return scene:findEntityByName("PlayerPrefab")
end

function rotateTowardsPlayer(dt)
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
    local rot = Quaternion.slerp(currentRot, targetQuat, dt * turn_speed)

    transform:setWorldRotation(rot)
end


function autoShootProjectile(dt)
    --if dt ~= nil then
    --    shot_timer = shot_timer + dt
     --   if shot_timer < shooting_speed then return end
    --    shot_timer = shot_timer - shooting_speed
    --end

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

function ballCheck()
    --log("Checking for ball...")
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local ball = scene:findEntityByName("Ball")
    if ball ~= nil then
        active = true
    end
end

function destroySelf()
    if destroyed then return end
    destroyed = true
    SceneManager:destroy(self)
end
