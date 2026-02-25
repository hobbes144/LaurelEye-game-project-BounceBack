destroyed = false

transform = nil
body = nil

initialSpeed = nil
bounceCount = 0.0

returnSpeed = nil

scene = nil
playerEntity = nil
playerTransform = nil

function onStart()
    transform = self:findTransform()
    body = self:findGhostBody()
    scene = SceneManager:getCurrentScene()
    playerEntity = scene:findEntityByName("PlayerPrefab")
    playerTransform = playerEntity and playerEntity:findTransform()
end

function onUpdate(dt)
    if initialSpeed == nil then
        local vel = body:getLinearVelocity()
        initialSpeed = vel:Magnitude()
        returnSpeed = initialSpeed + 50.0
    end
    if destroyed then return end
end

function onTriggerEnter(data)
    if destroyed then return end
    if body == nil then return end

    if (isGround(data.entityA) or isGround(data.entityB)) then

        print("Ball Has Hit Ground")

        -- After 2 bounces, home to player
        if bounceCount >= 2 then

            local ballPos = transform:getWorldPosition()
            local playerPos = playerTransform:getWorldPosition()
            playerPos = Vector3.new(playerPos.x, 4.5, playerPos.z)

            local dir = playerPos - ballPos

            if dir:Magnitude() > 0 then
                dir = dir:Normalized()
                body:setLinearVelocity(dir * returnSpeed)
            end
            return
        end

        local vel = body:getLinearVelocity()
        if vel:Magnitude() == 0 then return end

        -- Get collision normal
        local normal =
            data.normal or
            data.contactNormal or
            Vector3.new(0, 1, 0)

        normal = normal:Normalized()

        -- Reflect
        local reflected = vel - normal * (2.0 * vel:Dot(normal))
        reflected = reflected:Normalized()

        local pos = transform:getWorldPosition()
        local homingDir, hit = findEnemyDirection(pos, reflected)

        if homingDir ~= nil then
            reflected = homingDir
        end

        reflected = reflected * initialSpeed
        --print("Reflected To " + reflected)
        body:setLinearVelocity(reflected)

        print("Ball Has Bounced")
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
    local maxDistance = 200.0
    local coneAngle = math.rad(15)      -- cone half-angle
    local rings = 3                     -- quality vs cost
    local raysPerRing = 6

    baseDir = baseDir:Normalized()

    -- Build orthonormal basis
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
