defaultDistance = 22.0
aimDistance = 12.0

defaultShoulder = 6.5 
aimShoulder = 4.5   
defaultHeight = 8.5
aimHeight = 7.8
zoomLerpSpeed = 12.0

currentDistance = defaultDistance
currentShoulder = defaultShoulder
currentHeight = defaultHeight

transform = nil
target = nil

-- Rotation
yaw = 0.0
pitch = -0.25 -- slight downward angle

lerpSpeed = 10.0

-- Pitch clamp
pitchMin = -1.2
pitchMax = 0.35

-- Right-stick settings
rStickDeadzone = 0.25

mouseSensitivity = 0.002
stickSensitivity = 2.0

currentFov = 1.1

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

    local isAiming = Input:isMouseButtonHeld(MouseButton.Right)

    local targetDistance = defaultDistance
    local targetShoulder = defaultShoulder
    local targetHeight = defaultHeight

    local scene = SceneManager:getCurrentScene()
    local cameraEntity = scene:findEntityByName("Camera")
    local cameraComponent = cameraEntity:findComponent("CameraComponent")

    local targetFov = 1.1
    if isAiming then
        targetDistance = aimDistance
        targetShoulder = aimShoulder
        targetHeight = aimHeight

        targetFov = 0.6
    end

    currentFov = currentFov + (targetFov - currentFov) * zoomLerpSpeed * dt

    if cameraComponent ~= nil then
        cameraComponent:setFov(currentFov)
    end

    currentDistance =
        currentDistance + (targetDistance - currentDistance) * zoomLerpSpeed * dt

    currentShoulder =
        currentShoulder + (targetShoulder - currentShoulder) * zoomLerpSpeed * dt

    currentHeight =
        currentHeight + (targetHeight - currentHeight) * zoomLerpSpeed * dt

    -- Build camera basis
    local rotQuat = Quaternion.fromEuler(pitch, yaw, 0)
    local forward = rotQuat:forward()
    local right = rotQuat:right()

    -- Desired camera position (over-the-shoulder)
    local desiredPos =
        playerPos
        - forward * currentDistance
        + right * currentShoulder
        + Vector3.new(0, currentHeight, 0)

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
