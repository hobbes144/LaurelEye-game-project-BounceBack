transform = nil
target = nil

-- camera Path Spline
totalLength = 0.0
spline = nil
u = 0.0
speed = 0.0

-- Camera offsets
distanceBack = 100.0
height = 50.0
rotationSpeed = 1.0
lerpSpeed = 8.0

yaw = 0.0

-- Right-stick / camera control settings
rStickDeadzone = 0.25
heightSpeed = 15.0
heightMin = 5.0
heightMax = 80.0

function onStart()
    log("Camera script started")

    print("CatmullRomSpline type:", type(CatmullRomSpline))
    print("CatmullRomSpline value:", CatmullRomSpline)

    if CatmullRomSpline == nil then
        print("ERROR: CatmullRomSpline is nil - not registered!")
    end

    transform = self:findTransform()
    target = findPlayer()

    if transform == nil then
        logerr("Camera Transform Not Found")
        return
    end

    -- Create spline
    spline = CatmullRomSpline.new()
    --print("Vector2 type:", type(Vector2))
    --print("Vector2 value:", Vector2)

    if Vector2 then
        print("Vector2 metatable:")
        for k, v in pairs(getmetatable(Vector2) or {}) do
            print("  ", k, v)
        end
    end

    spline = CatmullRomSpline.new()

    local r = 20

    spline:set_control_points({
        Vector2.new( r, 0 ),
        Vector2.new( r * 0.707,  r * 0.707 ),
        Vector2.new( 0,  r ),
        Vector2.new( -r * 0.707,  r * 0.707 ),
        Vector2.new( -r, 0 ),
        Vector2.new( -r * 0.707, -r * 0.707 ),
        Vector2.new( 0, -r ),
        Vector2.new( r * 0.707, -r * 0.707 )
    })

    --spline:set_control_points({
        --Vector2.new(0, 0),
        --Vector2.new(30, 10),
        --Vector2.new(60, 40),
        --Vector2.new(100, 20),
        --Vector2.new(140, 50)
    --})

    -- Build arc-length table once
    totalLength = spline:total_length()

    print("Spline total length calculated:", totalLength)
    if totalLength == nil or totalLength <= 0 then
        logerr("Spline total length invalid")
        return
    end

    u = 0.0
    speed = 10.0

    print("Spline initialized. Total length = ", totalLength)
end

function onUpdate(dt)
    if spline == nil then
        print("Spline not initialized")
        return
    end

    if target == nil then
        target = findPlayer()
        if target == nil then return end
    end

    -- Right stick input
    local rStickX = Input:getGamepadAxis(GamepadAxes.RStickX)
    local rStickY = Input:getGamepadAxis(GamepadAxes.RStickY)

    if math.abs(rStickX) < rStickDeadzone then rStickX = 0.0 end
    if math.abs(rStickY) < rStickDeadzone then rStickY = 0.0 end

    local playerPos = target:getWorldPosition()
    local camTransform = transform:getWorldTransform()
    local camPos = camTransform:getPosition()

    -- Advance along spline
    u = u + speed * dt
    if u > totalLength then
        u = u - totalLength
    end


    local pos2 = spline:evaluate_arc_length(u)

    camPos = Vector3.new(pos2.x, 5.0, pos2.y)

    --print("Camera spline position:", camPos)

    -- Camera keyboard control
    if Input:isKeyHeld(Key.ArrowLeft) then yaw = yaw - rotationSpeed * dt end
    if Input:isKeyHeld(Key.ArrowRight) then yaw = yaw + rotationSpeed * dt end
    if Input:isKeyHeld(Key.ArrowUp) then height = height + heightSpeed * dt end
    if Input:isKeyHeld(Key.ArrowDown) then height = height - heightSpeed * dt end

    -- Right-stick adds camera control
    yaw = yaw + rStickX * rotationSpeed * dt
    height = height + rStickY * heightSpeed * dt

    -- Compute desired camera position (orbit logic)
    local offsetX = math.sin(yaw) * distanceBack
    local offsetZ = math.cos(yaw) * distanceBack
    local desiredPos = Vector3.new(
        playerPos.x - offsetX,
        playerPos.y + height,
        playerPos.z + offsetZ
    )

    -- Apply spline position (your choice)
    camTransform:setPosition(camPos)

    -- Look at player
    local dir = (playerPos - camPos):Normalized()

    local yawAngle = math.atan(-dir.x, -dir.z)
    local pitchAngle = math.asin(-dir.y)

    local targetQuat = Quaternion.fromEuler(-pitchAngle, yawAngle, 0)
    local currentRot = camTransform:getRotation()

    --print("Current rotation:", currentRot)
    --print("Target rotation:", targetQuat)

    local newRot = Quaternion.slerp(currentRot, targetQuat, dt * lerpSpeed)
    camTransform:setRotation(newRot)

    transform:setWorldTransform(camTransform)
end

function onShutdown()
    log("Camera controller stopped")
end

function findPlayer()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return nil end
    local playerEntity = scene:findEntityByName("PlayerPrefab")
    if playerEntity == nil then return nil end
    return playerEntity:findTransform()
end
