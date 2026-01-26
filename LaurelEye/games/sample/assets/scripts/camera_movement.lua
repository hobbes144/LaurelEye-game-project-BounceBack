transform = nil
target = nil

-- Camera offsets
distanceBack = 22.0
height = 8.0
shoulderOffset = 4.0 -- positive = right shoulder, negative = left

-- Rotation
yaw = 0.0
pitch = -0.25 -- slight downward angle

rotationSpeed = 2.5
lerpSpeed = 10.0

-- Pitch clamp
pitchMin = -1.2
pitchMax = 0.35

-- Right-stick settings
rStickDeadzone = 0.25

mouseSensitivity = 0.002
stickSensitivity = 2.0

function onStart()
    transform = self:findTransform()
    target = findPlayer()
end

function onUpdate(dt)
    if target == nil then
        target = findPlayer()
        if target == nil then return end
    end

    -- Mouse + controller look (FPS-style)
    local dx, dy = Input:getMouseDelta()

    local rStickX = Input:getGamepadAxis(GamepadAxes.RStickX)
    local rStickY = Input:getGamepadAxis(GamepadAxes.RStickY)

    if rStickX < 0.5 and rStickX > -0.5 then rStickX = 0 end
    if rStickY < 0.5 and rStickY > -0.5 then rStickY = 0 end

    yaw   = yaw   - (dx * mouseSensitivity) - (rStickX * stickSensitivity * dt)
    pitch = pitch - (dy * mouseSensitivity) - (rStickY * stickSensitivity * dt)

    pitch = math.max(pitchMin, math.min(pitchMax, pitch))

    local playerPos = target:getWorldPosition()
    local camTransform = transform:getWorldTransform()
    local camPos = camTransform:getPosition()

    -- Build camera basis
    local rotQuat = Quaternion.fromEuler(pitch, yaw, 0)
    local forward = rotQuat:forward()
    local right = rotQuat:right()

    -- Desired camera position (over-the-shoulder)
    local desiredPos =
        playerPos
        - forward * distanceBack
        + right * shoulderOffset
        + Vector3.new(0, height, 0)

    -- Smooth follow
    camPos.x = camPos.x + (desiredPos.x - camPos.x) * lerpSpeed * dt
    camPos.y = camPos.y + (desiredPos.y - camPos.y) * lerpSpeed * dt
    camPos.z = camPos.z + (desiredPos.z - camPos.z) * lerpSpeed * dt
    camTransform:setPosition(camPos)

    -- Camera looks forward (aim direction), not directly at player
    local targetRot = Quaternion.fromEuler(pitch, yaw, 0)
    local currentRot = camTransform:getRotation()
    local newRot = Quaternion.slerp(currentRot, targetRot, dt * lerpSpeed)

    camTransform:setRotation(newRot)
    transform:setWorldTransform(camTransform)
end

function onShutdown()
    log("Camera controller stopped")
end

-- Helper
function findPlayer()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return nil end
    local playerEntity = scene:findEntityByName("PlayerPrefab")
    if playerEntity == nil then return nil end
    return playerEntity:findTransform()
end
