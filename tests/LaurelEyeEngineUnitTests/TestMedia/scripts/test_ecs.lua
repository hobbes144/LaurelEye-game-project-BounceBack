function onStart()
    log("ECS Test: onStart called")
end

function onUpdate(dt)
    log("ECS Test: onUpdate called")

    -- Access our own entity
    local transform = self:findTransform()
    if transform then
        log("Found Transform component")

        -- For testing, assume Transform has position {x, y, z}
        local pos = transform:getWorldPosition()
        log(string.format("Old position: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z))

        -- Modify it (example)
        transform:setWorldPosition(pos.x + 1.0, pos.y + 0.5, pos.z)
        log("Updated Transform position from Lua")
    else
        log("Transform not found!")
    end
end
