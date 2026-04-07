destroyed = false

transform = nil
body = nil

initialSpeed = nil
bounceCount = 0

returnSpeed = nil

scene = nil
playerEntity = nil
playerTransform = nil

chaseRange = 10.0

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
    end
end

function onUpdate(dt)
    if initialSpeed == nil then
        local vel = body:getLinearVelocity()
        initialSpeed = vel:Magnitude()
        returnSpeed = initialSpeed + 25.0
    end

    local target = findPlayer()
    if not target then return end

    local pos = transform:getWorldPosition()
    local dir = target - pos
    dir.y = 0.0

    local dist = dir:Magnitude()
    if dist <= chaseRange then
        local n = dir:Normalized()

        local newVel = body:getLinearVelocity()
        local desiredVel = n * initialSpeed
        local ax = (desiredVel.x - newVel.x) * dt
        local az = (desiredVel.z - newVel.z) * dt

        newVel.x = newVel.x + ax
        newVel.z = newVel.z + az
        local horizSq = newVel.x*newVel.x + newVel.z*newVel.z
        if horizSq > initialSpeed * initialSpeed then
            local m = math.sqrt(horizSq)
            newVel.x = (newVel.x / m) * initialSpeed
            newVel.z = (newVel.z / m) * initialSpeed
        end

        body:setLinearVelocity(newVel)
    end
end

function onTriggerEnter(data)
    if body == nil then return end

    if (isGround(data.entityA) or isGround(data.entityB)) then

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

        if bounceCount == 1 then
            local message = Message.new()
            message.to = playerEntity
            message.topic = "FirstBounce"
            Script.send(message)
        end

        bounceCount = bounceCount + 1
    end
end

function onTriggerStay(data)
    local tagsA = data.entityA:getTags()
    local pos = transform:getWorldPosition()
    for _, tag in pairs(tagsA) do
        if tag == "ground" then
            transform:setWorldPosition(pos.x, 1.0, pos.z)
        end
    end
    local tagsB = data.entityB:getTags()
    for _, tag in pairs(tagsB) do
        if tag == "ground" then
            transform:setWorldPosition(pos.x, 1.0, pos.z)
        end
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
            GameManager:addGeneratorDestroyed(1)
            local gensDestroyed = GameManager:getGeneratorsDestroyed()
            print(gensDestroyed)
            return true
        end
        if tag == "enemy" then
            local message = Message.new()
            message.to = entity
            log("Get Script Hit")
            message.topic = "Get Hit!"
            Script.send(message)
            return true
        end
        if tag == "shield" then
            local message = Message.new()
            message.to = entity
            message.topic = "Bonk!"
            Script.send(message)
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
        if tag == "generator" then
            local message = Message.new()
            message.to = entity
            message.topic = "Get Destroyed!"
            Script.send(message)
            return true
        end
    end
    return false
end

function destroySelf()
    if destroyed then return end
    destroyed = true
    SceneManager:destroy(self)
end

function findPlayer()
    local scene = SceneManager:getCurrentScene()
    if not scene then return nil end

    local playerEntity = scene:findEntityByName("PlayerPrefab")
    if not playerEntity then return nil end

    local playerTransform = playerEntity:findTransform()
    if not playerTransform then return nil end

    return playerTransform:getWorldPosition()
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
