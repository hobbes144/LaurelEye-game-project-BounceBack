transform = nil
playerTransform = nil

mouseSensitivity = 0.002
stickSensitivity = 2.0
headHeight = 8.0
forwardOffset = 5.0

yaw = 0.0
pitch = 0.0

pitchMin = -1.3    -- -75°
pitchMax =  1.3    -- +75°

lastMouseX = 0
lastMouseY = 0
firstFrame = true

function onStart()
    transform = self:findTransform()
    if transform == nil then
        logErr("FPS Camera Transform not found")
        return
    end

    playerTransform = findPlayer()
    if playerTransform == nil then
        logerr("Player Transform Not Found")
    end

    -- Initialize mouse position
    local mx, my = Input:getMousePosition()
    lastMouseX = mx
    lastMouseY = my

    -- Initialize rotation from transform
    local rot = transform:getWorldRotation()
    local e = rot:toEuler()
    pitch = e.x
    yaw = e.y

end

function onUpdate(dt)
    if transform == nil then return end

    -- mouse input
    local mx, my = Input:getMousePosition()

    if firstFrame then
        lastMouseX = mx
        lastMouseY = my
        firstFrame = false
        return
    end

    local rStickX = Input:getGamepadAxis(GamepadAxes.RStickX)
    local rStickY = Input:getGamepadAxis(GamepadAxes.RStickY)

    if rStickX < 0.5 and rStickX > -0.5 then
        rStickX = 0.0
    end
    if rStickY < 0.5 and rStickY > -0.5 then
        rStickY = 0.0
    end

    local dx, dy = Input:getMouseDelta()

    lastMouseX = mx
    lastMouseY = my

    -- Update rotation
    yaw   = yaw   - (dx * mouseSensitivity) - (rStickX * stickSensitivity * dt)
    pitch = pitch - (dy * mouseSensitivity) - (rStickY * stickSensitivity * dt)


    -- Clamp pitch
    if pitch < pitchMin then pitch = pitchMin end
    if pitch > pitchMax then pitch = pitchMax end

    -- Apply rotation to camera
    local rot = Quaternion.fromEuler(pitch, yaw, 0)
    transform:setWorldRotation(rot)

    -- Position camera at head + forward offset
    local pos = playerTransform:getWorldPosition()
    pos.y = pos.y + headHeight

    local forward = rot:forward()
    pos = pos + forward * forwardOffset

    transform:setWorldPosition(pos)

end

function onShutdown()
    log("FPS Camera disabled")
end

function findPlayer()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return nil end
    local playerEntity = scene:findEntityByName("PlayerPrefab")
    if playerEntity == nil then return nil end
    return playerEntity:findTransform()
end
