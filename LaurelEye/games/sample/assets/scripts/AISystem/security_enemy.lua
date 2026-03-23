-- Security Enemy AI
-- Takes 3 hits to destroy, moves toward player instead of shooting

local StateMachine = require("StateMachine")
local State = require("State")

-- Enemy data
turnSpeed = 5.0
moveSpeed = 8.0
attackDistance = 8.0   -- Distance at which to switch to shield attack
transform = nil
body = nil
destroyed = false
hitCount = 0
maxHits = 3

-- State machine
enemyAI = nil

shield = nil

function onStart()
    transform = self:findTransform()
    body = self:findRigidBody()

    animator = self:findAnimator()
    if animator == nil then return end
    animator:changeAnimation("Security_Idle")

    shield = SceneManager:instantiate("prefabs/shield.prefab.json")
    local message = Message.new()
    message.to = shield
    message.from = self
    message.topic = "Here I Am!"
    Script.send(message)

    setupStateMachine()
end

function setupStateMachine()
    enemyAI = StateMachine.new()

    -- Create states
    local idle      = State.new("Idle")
    local alert     = State.new("Alert")
    local moving    = State.new("Moving")
    local attacking = State.new("Attacking")
    local getHit    = State.new("GetHit")
    local dead      = State.new("Dead")

    -- IDLE STATE
    idle:setOnEnter(function()
        log("Security: idle")
        if animator then animator:changeAnimation("Security_Idle") end
    end)

    idle:setUpdate(function(dt)
        -- Just wait
    end)

    idle:addTransition("toAlert", alert, 1, function()
        return getPlayer() ~= nil
    end)

    -- ALERT STATE
    -- Player has been spotted; pause briefly before pursuing
    alert:setOnEnter(function()
        log("Security: alert! Player spotted")
        alertTimer = 0.0
        if animator then animator:changeAnimation("Security_Idle") end
    end)

    alert:setUpdate(function(dt)
        alertTimer = alertTimer + dt
        -- Face the player while alerting
        rotateTowardsPlayer(dt)
    end)

    alert:addTransition("toMoving", moving, 1, function()
        return alertTimer ~= nil and alertTimer >= 1.5
    end)

    -- MOVING STATE
    -- Walk slowly toward the player
    moving:setOnEnter(function()
        log("Security: moving towards player")
        if animator then animator:changeAnimation("Running") end
    end)

    moving:setUpdate(function(dt)
        rotateTowardsPlayer(dt)
        moveTowardsPlayer(dt)
    end)

    moving:addTransition("toAttacking", attacking, 1, function()
        return isPlayerInRange(attackDistance)
    end)

    -- ATTACKING STATE
    -- Close enough: attack with shield
    attacking:setOnEnter(function()
        log("Security: attacking with shield!")
        if animator then animator:changeAnimation("Security_Idle") end
        local message = Message.new()
        message.to = shield
        message.topic = "Attack!"
        Script.send(message)
    end)

    attacking:setUpdate(function(dt)
        rotateTowardsPlayer(dt)
        -- Could add melee/shield damage logic here
    end)

    attacking:addTransition("toMoving", moving, 1, function()
        return not isPlayerInRange(attackDistance)
    end)

    -- GET HIT STATE
    -- Briefly stagger when hit; then resume moving
    getHit:setOnEnter(function()
        log("Security: got hit! Hits taken: " .. hitCount)
        hitTimer = 0.0
        if animator then animator:changeAnimation("Security_Idle") end
    end)

    getHit:setUpdate(function(dt)
        hitTimer = hitTimer + dt
    end)

    -- After stagger, resume chasing
    getHit:addTransition("toMoving", moving, 1, function()
        return hitTimer ~= nil and hitTimer >= 0.5
    end)

    -- DEAD STATE
    dead:setOnEnter(function()
        log("Security: destroyed after " .. maxHits .. " hits")
        if animator then animator:changeAnimation("StickManBadge_Death") end
        destroySelf()
    end)

    dead:setUpdate(function(dt) end)

    -- Register all states
    enemyAI:addState(idle)
    enemyAI:addState(alert)
    enemyAI:addState(moving)
    enemyAI:addState(attacking)
    enemyAI:addState(getHit)
    enemyAI:addState(dead)

    enemyAI:setInitialState("Idle")
end

function onMessage(msg)
    if msg.topic == "Get Hit!" then
        log("Security hit by ball!")
        hitCount = hitCount + 1
        if hitCount >= maxHits then
            if enemyAI then enemyAI:forceTransition("Dead") end
        else
            if enemyAI then enemyAI:forceTransition("GetHit") end
        end
        return true
    end
    if msg.topic == "We Bonked!" then
        if enemyAI then enemyAI:forceTransition("GetHit") end
        return true
    end
end

function onUpdate(dt)
    if enemyAI then
        enemyAI:update(dt)
    end
end

function onCollisionEnter(data)
    -- Check both entities in the collision for the "ball" tag
    local function checkTags(entity)
        local tags = entity:getTags()
        for _, tag in pairs(tags) do
            if tag == "ball" then
                log("Security hit by ball!")
                hitCount = hitCount + 1
                if hitCount >= maxHits then
                    if enemyAI then enemyAI:forceTransition("Dead") end
                else
                    if enemyAI then enemyAI:forceTransition("GetHit") end
                end
                return true
            end
        end
        return false
    end

    checkTags(data.entityA)
    checkTags(data.entityB)
end

function onCollisionStay(data) end
function onCollisionExit(data) end

-- ── Helper functions ──────────────────────────────────────────────────────────

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

    local selfPos   = transform:getWorldPosition()
    local playerPos = playerTransform:getWorldPosition()
    local distance  = (playerPos - selfPos):Magnitude()

    return distance < range
end

function rotateTowardsPlayer(dt)
    local player = getPlayer()
    if player == nil then return end

    local playerTransform = player:findTransform()
    if playerTransform == nil then return end
    if transform == nil then return end

    local selfPos   = transform:getWorldPosition()
    local playerPos = playerTransform:getWorldPosition()
    local dir = playerPos - selfPos
    dir.y = 0

    if dir:Magnitude() == 0 then return end
    dir = dir:Normalized()

    local frontOffset  = math.pi
    local angle        = math.atan(-dir.x, -dir.z) + frontOffset
    local targetQuat   = Quaternion.fromEuler(0, angle, 0)
    local currentRot   = transform:getWorldRotation()
    local rot          = Quaternion.slerp(currentRot, targetQuat, dt * turnSpeed)

    transform:setWorldRotation(rot)
end

function moveTowardsPlayer(dt)
    local player = getPlayer()
    if player == nil then return end

    local playerTransform = player:findTransform()
    if playerTransform == nil then return end
    if transform == nil then return end

    local selfPos   = transform:getWorldPosition()
    local playerPos = playerTransform:getWorldPosition()
    local dir = playerPos - selfPos
    dir.y = 0

    if dir:Magnitude() == 0 then return end
    dir = dir:Normalized()

    -- Use RigidBody velocity for physics-friendly movement
    if body ~= nil then
        local vel = dir * moveSpeed
        vel.y = body:getLinearVelocity().y  -- preserve gravity
        body:setLinearVelocity(vel)
    else
        -- Fallback: directly translate the transform
        local newPos = selfPos + dir * moveSpeed * dt
        transform:setWorldPosition(newPos.x, newPos.y, newPos.z)
    end
end

function destroySelf()
    if destroyed then return end
    destroyed = true
    local message = Message.new()
    message.to = shield
    message.topic = "We Die Together!"
    Script.send(message)
    SceneManager:destroy(self)
end
