destroyed = false

transform = nil
body = nil

initialSpeed = nil
bounceCount = 0.0

returnSpeed = nil

function onStart()
    transform = self:findTransform()
    body = self:findPhysics()

end

function onUpdate(dt)
    if initialSpeed == nil then
        local vel = body:getLinearVelocity()
        initialSpeed = vel:Magnitude()
        returnSpeed = initialSpeed + 50.0
        print(moveSpeed)
    end
    if destroyed then return end
end

function onCollisionEnter(data)
    if destroyed then return end
    if body == nil then return end

    if (isGround(data.entityA) or isGround(data.entityB)) then
        -- After 2 bounces, home to player
        if bounceCount >= 2 then
            local scene = SceneManager:getCurrentScene()
            if scene == nil then return nil end
            local playerEntity = scene:findEntityByName("PlayerPrefab")
            if playerEntity == nil then return nil end
            local player = playerEntity:findTransform()
            if player ~= nil then
                local ballPos = transform:getWorldPosition()
                local playerPos = player:getWorldPosition()

                local dir = playerPos - ballPos

                if dir:Magnitude() > 0 then
                    dir = dir:Normalized()
                    body:setLinearVelocity(dir * returnSpeed)
                end
            end
            return
        end

        local vel = body:getLinearVelocity()
        if vel:Magnitude() == 0 then return end

        -- Get collision normal
        local normal =
            data.normal or
            data.contactNormal or
            Vector3.new(0, 1, 0)

        normal = normal:Normalized()

        -- Reflect
        local reflected = vel - normal * (2.0 * vel:Dot(normal))

        -- Enforce perfect elasticity
        reflected = reflected:Normalized() * initialSpeed
        body:setLinearVelocity(reflected)

        bounceCount = bounceCount + 1
    end
end

function isGround(entity)
    if entity == nil then return false end
    local tags = entity:getTags()
    for _, tag in pairs(tags) do
        if tag == "ground" then
            return true
        end
        if tag == "target" then
            return true
        end
        if tag == "player" then
            bounceCount = 0
        end
    end
    return false
end

function destroySelf()
    if destroyed then return end
    destroyed = true
    SceneManager:destroy(self)
end
