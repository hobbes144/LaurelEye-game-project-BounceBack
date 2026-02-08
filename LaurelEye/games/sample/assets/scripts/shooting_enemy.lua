turnSpeed   = 8.0       -- how fast the enemy rotates toward movement

transform = nil
body = nil
destroyed = false

shotTimer = 0.0
shootingSpeed = 3.0 -- Default value
minShootingSpeed = 2.5
maxShootingSpeed = 4.0

function onStart()
    transform = self:findTransform()
    body = self:findPhysics()

    shootingSpeed = math.random(minShootingSpeed * 100, maxShootingSpeed * 100) / 100


end

function onUpdate(dt)
    rotateTowardsPlayer(dt)
    autoShootProjectile(dt)
end

function onCollisionEnter(data)
    local tagsA = data.entityA:getTags()
    for _, tag in pairs(tagsA) do
        if tag == "ball" then
            print("Collided with Ball!")
            destroySelf()
        end
    end

    local tagsB = data.entityB:getTags()
    for _, tag in pairs(tagsB) do
        if tag == "ball" then
            print("Collided with Ball!")
            destroySelf()
        end
    end    
end

function onCollisionStay(data) end
function onCollisionExit(data) end

function rotateTowardsPlayer(dt)
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end

    local player = scene:findEntityByName("PlayerPrefab")
    if player == nil then return end

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
    local rot = Quaternion.slerp(currentRot, targetQuat, dt * turnSpeed)

    transform:setWorldRotation(rot)
end

function autoShootProjectile(dt)
    if dt ~= nil then
        shotTimer = shotTimer + dt
        if shotTimer < shootingSpeed then return end
        shotTimer = shotTimer - shootingSpeed
    end

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

    local projBody = proj:findPhysics()
    if projBody ~= nil then
        local projectileSpeed = 100.0 -- tune as needed
        projBody:setLinearVelocity(dir * projectileSpeed)
    end
end

function destroySelf()
    if destroyed then return end
    destroyed = true
    SceneManager:destroy(self)
end
