transform = nil
speed = 20.0

function onStart()
    transform = self:findTransform()
    if transform == nil then
        logerr("Camera Transform Not Found")
     end
end

function onUpdate(dt)
    local moveX = 0.0
    local moveZ = 0.0

    if Input:isKeyHeld(Key.ArrowLeft) then
        moveX = moveX - 1.0
    end
    if Input:isKeyHeld(Key.ArrowRight) then
        moveX = moveX + 1.0
    end
    if Input:isKeyHeld(Key.ArrowDown) then
        moveZ = moveZ + 1.0
    end
    if Input:isKeyHeld(Key.ArrowUp) then
        moveZ = moveZ - 1.0
    end
    
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
    log("Camera controller stopped")
end
