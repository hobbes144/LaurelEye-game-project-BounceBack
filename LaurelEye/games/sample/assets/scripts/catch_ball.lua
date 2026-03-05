destroyed = false

transform = nil
body = nil

initialSpeed = nil
bounceCount = 0

returnSpeed = nil

scene = nil
playerEntity = nil
playerTransform = nil

gravityDisabled = false
homingTimer = nil        -- tracks time since homing started
HOMING_TIMEOUT = 2.0     -- seconds before giving up and sending "Catch Me!"

function onStart()
    transform = self:findTransform()
    body = self:findGhostBody()
    scene = SceneManager:getCurrentScene()
    playerEntity = scene:findEntityByName("PlayerPrefab")
    playerTransform = playerEntity and playerEntity:findTransform()
end

function onMessage(msg)
    if msg.topic == "I am kicking you!" then
        print("Bouncing Away!")
        bounceCount = 0
        gravityDisabled = false
        homingTimer = nil

        -- restore gravity if needed
        local data = body:getBodyData()
        data.gravityScale = 1.0
        body:setBodyData(data)
    end
end

function onUpdate(dt)
    if initialSpeed == nil then
        local vel = body:getLinearVelocity()
        initialSpeed = vel:Magnitude()
        returnSpeed = initialSpeed + 25.0
    end

    -- Count down the homing timeout
    if homingTimer ~= nil then
        homingTimer = homingTimer + dt
        if homingTimer >= HOMING_TIMEOUT then
            print("Homing timeout! Sending Catch Me!")
            local message = Message.new()
            message.to = playerEntity
            message.topic = "Catch Me!"
            Script.send(message)
            destroySelf()
        end
    end
end

function onTriggerEnter(data)
    if body == nil then return end

    if (isGround(data.entityA) or isGround(data.entityB)) then
        
        -- After 2 bounces, home to player
        if bounceCount >= 2 then

            local ballPos = transform:getWorldPosition()
            local playerPos = playerTransform:getWorldPosition()
            playerPos = Vector3.new(playerPos.x, 4.5, playerPos.z)
            local newGrav = Vector3.new(0.0, 0.0, 0.0)
            body:setGravityScale(newGrav)

            local dir = playerPos - ballPos

            if dir:Magnitude() > 0 then
                dir = dir:Normalized()
                body:setLinearVelocity(dir * returnSpeed)
            end

            -- Start the homing timer if not already running
            if homingTimer == nil then
                homingTimer = 0.0
            end

            return
        end

        local vel = body:getLinearVelocity()
        local normal = data.normal or data.contactNormal
        if normal == nil then return end

        normal = normal:Normalized()
        local reflected = vel - normal * (2.0 * vel:Dot(normal))

        local pos = transform:getWorldPosition()
        local homingDir = findEnemyDirection(pos, reflected)
        if homingDir ~= nil then
            reflected = homingDir * reflected:Magnitude()
        end

        body:setLinearVelocity(reflected)

        bounceCount = bounceCount + 1
    end
end

function isGround(entity)
    if entity == nil then return false end
    local tags = entity:getTags()
    for _, tag in pairs(tags) do
        if tag == "ground" then
            return true
        end
        if tag == "target" then
            SceneManager:destroy(entity)
            return true
        end
        if tag == "door" then
            return true
        end
        if tag == "player" then
            local message = Message.new()
            message.to = playerEntity
            message.topic = "Catch Me!"
            Script.send(message)
            SceneManager:destroy(self)
        end
    end
    return false
end

function destroySelf()
    if destroyed then return end
    destroyed = true
    SceneManager:destroy(self)
end

function findEnemyDirection(origin, baseDir)
    local maxDistance = 75.0
    local coneAngle = math.rad(15)
    local rings = 3
    local raysPerRing = 6

    baseDir = baseDir:Normalized()

    local up = Vector3.new(0, 1, 0)
    if math.abs(baseDir:Dot(up)) > 0.95 then
        up = Vector3.new(1, 0, 0)
    end

    local right = baseDir:Cross(up):Normalized()
    up = right:Cross(baseDir):Normalized()

    for r = 1, rings do
        local t = r / rings
        local angle = coneAngle * t

        for i = 1, raysPerRing do
            local theta = (i / raysPerRing) * math.pi * 2.0

            local offset =
                right * math.cos(theta) * math.sin(angle) +
                up    * math.sin(theta) * math.sin(angle)

            local dir = (baseDir * math.cos(angle) + offset):Normalized()

            local hit = Physics.Raycast(origin, dir, maxDistance, { layerMask = Layers.Enemy })

            if hit then
                return dir, hit
            end
        end
    end

    return nil, nil
end
