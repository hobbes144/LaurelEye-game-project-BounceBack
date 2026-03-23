securityEnemy = nil
shieldTransform = nil
enemyTransform = nil

-- Tweak these to adjust shield placement
local forwardOffset = -1.5
local heightOffset  = 4.2

-- Lunge attack settings
local isLunging      = false
local lungeTimer     = 0.0
local lungeDuration  = 0.4   -- Total time for the full lunge out + back
local lungeDistance  = 5.0   -- How far forward the lunge travels

function onStart()
    shieldTransform = self:findTransform()
end

function onMessage(msg)
    if msg.topic == "Here I Am!" then
        securityEnemy = msg.from
        enemyTransform = securityEnemy:findTransform()
        log("Shield: linked to security enemy")
    end

    if msg.topic == "We Die Together!" then
        SceneManager:destroy(self)
    end

    if msg.topic == "Bonk!" then
        local message = Message.new()
        message.to = securityEnemy
        message.topic = "We Bonked!"
        Script.send(message)
        return true
    end

    if msg.topic == "Attack!" then
        -- Start a lunge if not already lunging
        if not isLunging then
            isLunging   = true
            lungeTimer  = 0.0
            log("Shield: lunging!")
        end
    end
end

function onUpdate(dt)
    if enemyTransform == nil then return end
    if shieldTransform == nil then return end

    -- Get enemy position and facing direction
    local enemyPos = enemyTransform:getWorldPosition()
    local enemyRot = enemyTransform:getWorldRotation()
    local forward  = enemyRot:forward()
    forward.y = 0

    if forward:Magnitude() > 0 then
        forward = forward:Normalized()
    end

    -- Calculate lunge offset using a sine curve (0 → peak → 0)
    local extraLunge = 0.0
    if isLunging then
        lungeTimer = lungeTimer + dt
        local progress = lungeTimer / lungeDuration  -- 0.0 to 1.0

        if progress >= 1.0 then
            -- Lunge finished, snap back to rest
            isLunging  = false
            lungeTimer = 0.0
            extraLunge = 0.0
        else
            -- Sine gives a smooth out-and-back arc over the duration
            extraLunge = math.sin(-progress * math.pi) * lungeDistance
        end
    end

    -- Place shield in front of enemy (plus any lunge offset)
    local shieldPos = enemyPos + forward * (forwardOffset + extraLunge)
    shieldPos.y = shieldPos.y + heightOffset

    shieldTransform:setWorldPosition(shieldPos.x, shieldPos.y, shieldPos.z)
    shieldTransform:setWorldRotation(enemyRot)
end

function onCollisionEnter(data)
    local tagsA = data.entityA:getTags()
    for _, tag in pairs(tagsA) do
        if tag == "player" then
            local scene = SceneManager:getCurrentScene()
            local player = scene:findEntityByName("PlayerPrefab")
            if player == nil then return end
            local message = Message.new()
            message.to = player
            message.topic = "Get Hit!"
            Script.send(message)
        end
    end

    local tagsB = data.entityB:getTags()
    for _, tag in pairs(tagsB) do
        if tag == "player" then
            local scene = SceneManager:getCurrentScene()
            local player = scene:findEntityByName("PlayerPrefab")
            if player == nil then return end
            local message = Message.new()
            message.to = player
            message.topic = "Get Hit!"
            Script.send(message)
        end
    end

end
