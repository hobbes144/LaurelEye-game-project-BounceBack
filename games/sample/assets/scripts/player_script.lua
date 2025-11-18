maxSpeed       = 40.0      -- units per second
accelGround    = 50.0      -- player accelerates FAST on ground
accelAir       = 30.0      -- player accelerates slower in air
decelGround    = 5.0       -- the rate at which we slow down while on ground
jumpVelocity   = 60.5 
jumpCut        = 0.45      -- Player loses this percentage upward velocity on release
turnSpeed      = 10.0      -- the rate at which the player turns

transform = nil
body = nil
cameraTransform = nil

isGrounded = true
jumping = false

function onStart()
    transform = self:findTransform()
    body = self:findPhysics()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return nil end
    local cameraEntity = scene:findEntityByName("Camera")
    if cameraEntity == nil then return nil end
    cameraTransform = cameraEntity:findTransform()
end

function onUpdate(dt)
    local inputX = 0.0
    local inputZ = 0.0

    if Input:isKeyHeld(Key.A) then inputX = inputX - 1 end
    if Input:isKeyHeld(Key.D) then inputX = inputX + 1 end
    if Input:isKeyHeld(Key.W) then inputZ = inputZ - 1 end
    if Input:isKeyHeld(Key.S) then inputZ = inputZ + 1 end

    -- Normalize input
    local mag = math.sqrt(inputX*inputX + inputZ*inputZ)
    if mag > 0 then
        inputX = inputX / mag
        inputZ = inputZ / mag
    end

    -- Locate current velocity
    local vel = body:getLinearVelocity()

    -- determine acceleration and deceleration dependent on state
    local accel = isGrounded and accelGround or accelAir
    local decel = decelGround

    -- acceleration based velocity change
    local ax = 0.0
    local az = 0.0

    local moveDir = getCameraRelativeMovement(inputX, inputZ)

    -- Input is pressed
    if mag > 0 then
        -- Desired velocity
        local desiredX = moveDir.x * maxSpeed
        local desiredZ = moveDir.z * maxSpeed

        -- Slowly adjust velocity towards desired
        ax = (desiredX - vel.x) * accel * dt
        az = (desiredZ - vel.z) * accel * dt
    else
        -- No input, decelerate
        ax = -vel.x * decel * dt
        az = -vel.z * decel * dt
    end

    -- Apply horizontal vel
    vel.x = vel.x + ax
    vel.z = vel.z + az

    -- Clamp horizontal speed
    local horizSq = vel.x*vel.x + vel.z*vel.z
    if horizSq > maxSpeed * maxSpeed then
        local m = math.sqrt(horizSq)
        vel.x = (vel.x / m) * maxSpeed
        vel.z = (vel.z / m) * maxSpeed
    end

    -- Jump
    if isGrounded and Input:isKeyPressed(Key.Space) then
        local mass = body:getBodyData().mass or 1.0  -- fallback if mass unknown
        body:applyForce(Vector3.new(0.0, jumpVelocity * mass / dt, 0.0)) 
        jumping = true
        isGrounded = false
    end

    -- Early jump release
    if jumping and not Input:isKeyHeld(Key.Space) and vel.y > 0 then
        vel.y = vel.y * jumpCut
        jumping = false
    end

    -- === Rotation ===
    if mag > 0 then
        local angle = math.atan(-moveDir.x, -moveDir.z)
        rotateTo(angle, dt)
    else
        -- face motion direction slightly when sliding
        local v = body:getLinearVelocity()
        local s = v.x*v.x + v.z*v.z
        if s > 1.0 then
            local slideAngle = math.atan(-v.x, -v.z)
            rotateTo(slideAngle, dt)
        end
    end

    body:setLinearVelocity(vel)
end

function getCameraRelativeMovement(inputX, inputZ)
    if cameraTransform == nil then
        return Vector3.new(inputX, 0, inputZ) -- fallback
    end

    -- Camera forward vector (flattened)
    local camRot = cameraTransform:getWorldRotation()
    local forward = camRot:forward()
    forward.y = 0
    if forward:Magnitude() > 0 then
        forward = forward:Normalized()
    end

    -- Camera right vector
    local right = camRot:right()
    right.y = 0
    if right:Magnitude() > 0 then
        right = right:Normalized()
    end

    -- Combine input
    local moveDir = forward * inputZ + right * inputX
    if moveDir:Magnitude() > 0 then
        moveDir = moveDir:Normalized()
    end
    return moveDir
end


function rotateTo(angle, dt)
   local target = Quaternion.fromEuler(0, angle, 0)
    local current = transform:getWorldRotation()

    -- smooth turn
    local rot = Quaternion.slerp(current, target, dt * turnSpeed)

    transform:setWorldRotation(rot)
end

function onCollisionEnter(data)
    -- Todo - add tag based check
    local name = data.entityB:getName()
    if name == "Ground" then
        isGrounded = true
        jumping = false
    end
end

function onCollisionExit(data)
    -- Todo - add tag based check
    local name = data.entityB:getName()
    if name == "Ground" then
        isGrounded = false
    end
end
