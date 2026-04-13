destroyed = false

transform = nil
body = nil

initialSpeed = nil
bounceCount = 0

returnSpeed = nil

scene = nil
playerEntity = nil
playerTransform = nil

impactPE = nil
attackPE = nil
kickbackPE = nil

pickupRange = 1.5      -- distance at which ball is instantly caught
chaseRange = 10.0
chaseSpeed = 200.0     -- how fast it moves toward player when homing

chaseRange = 10.0
pickupDelay = 1.0      -- seconds before pickup range activates
pickupTimer = 0.0

function onStart()
    transform = self:findTransform()
    body = self:findGhostBody()
    scene = SceneManager:getCurrentScene()
    playerEntity = scene:findEntityByName("PlayerPrefab")
    playerTransform = playerEntity and playerEntity:findTransform()

    local impactEntity = scene:findEntityByName("ImpactEmitter")
    impactPE = impactEntity and impactEntity:findParticleEmitter()
    impactPE:pause()

    local attackEntity = scene:findEntityByName("AttackEmitter")
    attackPE = attackEntity and attackEntity:findParticleEmitter()
    attackPE:pause()

    local kickbackEntity = scene:findEntityByName("KickbackEmitter")
    kickbackPE = kickbackEntity and kickbackEntity:findParticleEmitter()
    kickbackPE:pause()

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

    local pos = transform:getWorldPosition()

    if pos.y < 0.5 then
        local message = Message.new()
        message.to = playerEntity
        message.topic = "Catch Me!"
        Script.send(message)
        SceneManager:destroy(self)
        return
    end

    pickupTimer = pickupTimer + dt

    local target = findPlayer()
    if not target then return end


        local dir = (target - pos)
    local dist = dir:Magnitude()

    if pickupTimer <= pickupDelay then return end

    if dist <= pickupRange then
        local message = Message.new()
        message.to = playerEntity
        message.topic = "Catch Me!"
        Script.send(message)
        SceneManager:destroy(self)
        return
    end

    if dist > chaseRange then return end

    local n = dir:Normalized()
    local vel = body:getLinearVelocity()
    vel.x = n.x * chaseSpeed
    vel.y = n.y * chaseSpeed
    vel.z = n.z * chaseSpeed
    body:setLinearVelocity(vel)
end

function onTriggerEnter(data)
    if body == nil then return end

    local function processTags(entity)
        if entity == nil then return false end
        local tags = entity:getTags()
        for _, tag in pairs(tags) do
            if tag == "enemy" then
                local message = Message.new()
                message.to = entity
                message.topic = "Get Hit!"
                impactPE:playFor(0.1)
                attackPE:playFor(0.2)
                Script.send(message)
                return true
            end
            if tag == "ground" then
                impactPE:playFor(0.1)
                return true
            end
            if tag == "door" then
                impactPE:playFor(0.1)
                return true
            end
            if tag == "shield" then
                local message = Message.new()
                message.to = entity
                message.topic = "Bonk!"
                Script.send(message)
                return true
            end
            if tag == "player" then
                local message = Message.new()
                message.to = playerEntity
                message.topic = "Catch Me!"
                Script.send(message)
                SceneManager:destroy(self)
                return true
            end
            if tag == "target" then
                SceneManager:destroy(entity)
                GameManager:addGeneratorDestroyed(1)
                attackPE:playFor(0.1)
                impactPE:playFor(0.1)
                return true
            end
            if tag == "generator" then
                local message = Message.new()
                message.to = entity
                message.topic = "Get Destroyed!"
                Script.send(message)
                
                impactPE:playFor(0.1)
                attackPE:playFor(0.2)
                return true
            end
        end
        return false
    end

    -- Handle non-ground entity first
    if not processTags(data.entityA) then
        processTags(data.entityB)
    end

    -- Ground bounce is separate
    --if (isGround(data.entityA) or isGround(data.entityB)) then
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
    --end
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
