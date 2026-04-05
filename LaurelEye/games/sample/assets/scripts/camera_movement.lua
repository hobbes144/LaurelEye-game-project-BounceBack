defaultDistance = 22.0
aimDistance = 12.0

defaultShoulder = 6.5
aimShoulder = 8.5
defaultHeight = 8.5
aimHeight = 7.8
zoomLerpSpeed = 12.0
playerOffset = 0.0
playerOffsetZoomMult = 0.3
playerRunOffset = 4
playerIdleOffset = 0.0

currentDistance = defaultDistance
currentShoulder = defaultShoulder
currentHeight = defaultHeight

--@type TransformComponent
local transform = nil
--@type TransformComponent
local target = nil

-- Rotation
yaw = 0.0
pitch = -0.25 -- slight downward angle

lerpSpeed = 10.0
adsLerpSpeed = 40.0

-- Pitch clamp
pitchMin = -1.2
pitchMax = 1.2

-- Right-stick settings
rStickDeadzone = 0.25

mouseSensitivity = 0.002
stickSensitivity = 2.0

currentFov = 1.1

lockedPos = nil
locked = false

pivotPos = nil
function onStart()
    transform = self:findTransform()
    target = findPlayer()
    pivotPos = target:getWorldPosition()
end

function onUpdate(dt)
    if target == nil then
        target = findPlayer()
        if target == nil then return end
    end

    -- Mouse + controller look
    local dx, dy = Input:getMouseDelta()

    local rStickX = Input:getGamepadAxis(GamepadAxes.RStickX)
    local rStickY = Input:getGamepadAxis(GamepadAxes.RStickY)

    if rStickX < 0.5 and rStickX > -0.5 then rStickX = 0 end
    if rStickY < 0.5 and rStickY > -0.5 then rStickY = 0 end

    yaw   = yaw   - (dx * mouseSensitivity) - (rStickX * stickSensitivity * dt)
    pitch = pitch - (dy * mouseSensitivity) - (rStickY * stickSensitivity * dt)

    pitch = math.max(pitchMin, math.min(pitchMax, pitch))


    local playerPos = target:getWorldPosition()
    local playerRot = target:getWorldRotation()
    local camTransform = transform:getWorldTransform()

    -- Check if aiming
    local isAiming = Input:isMouseButtonHeld(MouseButton.Right)

    -- Set up base values
    local targetDistance = defaultDistance
    local targetShoulder = defaultShoulder
    local targetHeight = defaultHeight

    -- Get entities for reference
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

    -- Build rotation basis
    local rotQuat = Quaternion.fromEuler(pitch, yaw, 0)
    local forward = rotQuat:forward()
    local right = rotQuat:right()

    -- Target pivot follows player
    local cameraPivot =
        playerPos
        + Vector3.new(0, currentHeight, 0)

    if isAiming then
        cameraPivot = cameraPivot + ((playerRot * Vector3.new(0.0, 0.0, playerOffset)) * playerOffsetZoomMult)
    else
        cameraPivot = cameraPivot + (playerRot * Vector3.new(0.0, 0.0, playerOffset))
    end

    if isAiming then
        -- pivotPos = pivotPos + (cameraPivot - pivotPos) * adsLerpSpeed * dt
        pivotPos = cameraPivot
    else
        pivotPos = pivotPos + (cameraPivot - pivotPos) * lerpSpeed * dt
    end

    pivotPos = pivotPos + (cameraPivot - pivotPos) * lerpSpeed * dt

    -- Camera position derived from pivot
    local desiredPos =
        pivotPos
        - forward * currentDistance
        + right * (currentShoulder - 3.0)

    -- Raycasting to prevent clipping
    local hit = Physics.Raycast(
        pivotPos,
        desiredPos,
        { layerMask = Layers.World }
    )

    local padding = 0.3
    if hit then
        desiredPos = hit.position + hit.normal * padding
    end

    camTransform:setPosition(desiredPos)
    camTransform:setRotation(rotQuat)
    transform:setWorldTransform(camTransform)
end

function onMessage(message)
    if message.topic == "PlayerAnimChange" then
        if message.contents == "Running" then
            playerOffset = playerRunOffset
            debugLog("Camera offset set to playerRunOffset.")
        else
            playerOffset = playerIdleOffset
            debugLog("Camera offset set to playerIdleOffset.")
        end
    end
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
