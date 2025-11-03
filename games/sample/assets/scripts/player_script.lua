-- Player Controller Script
speed = 10.0
transform = nil

function onStart()
    log("Player controller started")
    transform = self:findTransform()
    if transform == nil then
        logerr("No transform found on entity!")
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

    -- Normalize diagonal movement
    local mag = math.sqrt(moveX * moveX + moveZ * moveZ)
    if mag > 0 then
        moveX = moveX / mag
        moveZ = moveZ / mag
    end

    -- Apply delta movement
    if moveX ~= 0 or moveZ ~= 0 then
        local pos = transform:getWorldPosition()
        pos.x = pos.x + moveX * speed * dt
        pos.z = pos.z + moveZ * speed * dt
        transform:setWorldPosition(pos)
    end
end

function onShutdown()
    log("Player controller stopped")
end
