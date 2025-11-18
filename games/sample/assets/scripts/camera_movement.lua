transform = nil
target = nil

-- Camera offsets
distanceBack = 35.0
height = 20.0
rotationSpeed = 1.0  -- radians per second
lerpSpeed = 8.0      -- smooth follow speed

yaw = 0.0 -- camera rotation around player

function onStart()
    transform = self:findTransform()
    target = findPlayer()
    if transform == nil then
        logerr("Camera Transform Not Found")
    end
end

function onUpdate(dt)
    if target == nil then
        target = findPlayer()
        if target == nil then return end
    end

    local playerPos = target:getWorldPosition()
    local camPos = transform:getWorldPosition()

    -- Camera controlling input
    if Input:isKeyHeld(Key.ArrowLeft) then
        yaw = yaw + rotationSpeed * dt  
    end
    if Input:isKeyHeld(Key.ArrowRight) then
        yaw = yaw - rotationSpeed * dt
    end

    -- Compute desired camera position
    local offsetX = math.sin(yaw) * distanceBack
    local offsetZ = math.cos(yaw) * distanceBack
    local desiredPos = Vector3.new(playerPos.x + offsetX, playerPos.y + height, playerPos.z - offsetZ)

    --interpolate to desired position
    camPos.x = camPos.x + (desiredPos.x - camPos.x) * lerpSpeed * dt
    camPos.y = camPos.y + (desiredPos.y - camPos.y) * lerpSpeed * dt
    camPos.z = camPos.z + (desiredPos.z - camPos.z) * lerpSpeed * dt
    transform:setWorldPosition(camPos)

    -- Look at player
    local dir = playerPos - camPos
    dir = dir:Normalized()

    local yawAngle = math.atan(-dir.x, -dir.z)
    local pitchAngle = math.asin(dir.y)

    local targetQuat = Quaternion.fromEuler(pitchAngle, yawAngle, 0)

    -- Smoothly interpolate rotation to remove jitter
    local currentRot = transform:getWorldRotation()
    local newRot = Quaternion.slerp(currentRot, targetQuat, dt * lerpSpeed)
    transform:setWorldRotation(newRot)
end

function onShutdown()
    log("Camera controller stopped")
end

-- Helper to find player (same as enemy)
function findPlayer()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return nil end
    local playerEntity = scene:findEntityByName("PlayerPrefab")
    if playerEntity == nil then return nil end
    return playerEntity:findTransform()
end
