transform = nil
target = nil

-- Camera offsets
distanceBack = 35.0
height = 20.0
rotationSpeed = 1.0  -- radians per second
lerpSpeed = 8.0      -- smooth follow speed

yaw = 0.0 -- camera rotation around player

-- Right-stick / camera control settings
rStickDeadzone = 0.25
heightSpeed = 15.0
heightMin = 5.0
heightMax = 80.0

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

    local rStickX = Input:getGamepadAxis(GamepadAxes.RStickX)
    local rStickY = Input:getGamepadAxis(GamepadAxes.RStickY)

    if rStickX < 0.5 and rStickX > -0.5 then
        rStickX = 0.0
    end
    if rStickY < 0.5 and rStickY > -0.5 then
        rStickY = 0.0
    end

    local playerPos = target:getWorldPosition()
    -- print("Player position: ", playerPos)
    local camTransform = transform:getWorldTransform()
    local camPos = camTransform:getPosition()
    -- print("Camera position: ", camPos)

    -- Camera controlling input (keyboard)
    if Input:isKeyHeld(Key.ArrowLeft) then
        yaw = yaw - rotationSpeed * dt
    end
    if Input:isKeyHeld(Key.ArrowRight) then
        yaw = yaw + rotationSpeed * dt
    end
    if Input:isKeyHeld(Key.ArrowUp) then
        height = height + heightSpeed * dt
    end
    if Input:isKeyHeld(Key.ArrowDown) then
        height = height - heightSpeed * dt
    end

    -- Right-stick adds camera control:
    yaw = yaw + rStickX * rotationSpeed * dt
    height = height + rStickY * heightSpeed * dt


    -- Compute desired camera position
    local offsetX = math.sin(yaw) * distanceBack
    local offsetZ = math.cos(yaw) * distanceBack
    local desiredPos = Vector3.new(playerPos.x - offsetX, playerPos.y + height, playerPos.z + offsetZ)

    --interpolate to desired position
    camPos.x = camPos.x + (desiredPos.x - camPos.x) * lerpSpeed * dt
    camPos.y = camPos.y + (desiredPos.y - camPos.y) * lerpSpeed * dt
    camPos.z = camPos.z + (desiredPos.z - camPos.z) * lerpSpeed * dt
    camTransform:setPosition(camPos)

    -- Look at player
    local dir = playerPos - camPos
    --print("Camera direction: ", dir)
    dir = dir:Normalized()

    local yawAngle = math.atan(-dir.x, -dir.z)
    local pitchAngle = math.asin(-dir.y)

    local targetQuat = Quaternion.fromEuler(-pitchAngle, yawAngle, 0)

    -- Smoothly interpolate rotation to remove jitter
    local currentRot = camTransform:getRotation()
    print("Current rotation: ", currentRot)
    print("Target rotation: ", targetQuat)
    local newRot = Quaternion.slerp(currentRot, targetQuat, dt * lerpSpeed)
    camTransform:setRotation(newRot)
    transform:setWorldTransform(camTransform)
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
