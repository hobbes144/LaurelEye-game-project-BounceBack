-- Player Controller Script
speed = 10.0
transform = nil

function onStart()
    log("Player controller started")
    transform = self:findTransform()
    body = self:findPhysics()

    if transform == nil then
        logerr("No transform found on entity!")
    end
    if body == nil then
        logerr("No PhysicsBodyComponent found on entity!")
    end
end

function onUpdate(dt)
    -- Movement input
    local moveX = 0.0
    local moveZ = 0.0
    
    -- This is wrong due to our axes being off.
    -- fix when axes are corrected
    if Input:isKeyHeld(Key.A) then
        moveZ = moveZ - 1.0
    end
    if Input:isKeyHeld(Key.D) then
        moveZ = moveZ + 1.0
    end
    if Input:isKeyHeld(Key.S) then
        moveX = moveX - 1.0
    end
    if Input:isKeyHeld(Key.W) then
        moveX = moveX + 1.0
    end
    if Input:isKeyPressed(Key.Space) then
        body:applyImpulse(0.0, 15.0, 0.0)
    end

    -- Normalize diagonal movement
    local mag = math.sqrt(moveX * moveX + moveZ * moveZ)
    if mag > 0 then
        moveX = moveX / mag
        moveZ = moveZ / mag
    end

    -- Apply physics force
    if moveX ~= 0 or moveZ ~= 0 then
        body:applyForce(moveX * speed, 0.0, moveZ * speed)
    end
end

function onShutdown()
    log("Player controller stopped")
end


function onCollisionEnter(data)
    log("[Lua] Collision enter with " .. data.entityB:getName())
    log("Contact point:", data.contactPoint.x, data.contactPoint.y, data.contactPoint.z)
end

function onCollisionStay(data)
    -- log("[Lua] Still colliding with " .. data.entityB:getName())
end

function onCollisionExit(data)
    log("[Lua] Collision exit with " .. data.entityB:getName())
end
