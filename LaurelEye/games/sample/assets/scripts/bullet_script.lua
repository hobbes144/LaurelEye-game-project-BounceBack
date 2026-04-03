lifeTime = 3.0          -- total lifespan before auto-destruction
destroyed = false

timeAlive = 0.0
transform = nil
body = nil

speaker = nil

bounceCount = 0
attackEnemy = false

function onStart()
    transform = self:findTransform()
    body = self:findGhostBody()

    --speaker = self:findAudioSpeaker()
    --if speaker ~= nil then
        --speaker:stop()
        --speaker:play()
    --end
end

function onMessage(msg)
    if msg.topic == "I am kicking you!" then
        bounceCount = 3
        attackEnemy = true
    end
end

function onUpdate(dt)
    if destroyed then return end

    timeAlive = timeAlive + dt

    -- Auto-destroy
    if timeAlive >= lifeTime then
        destroySelf()
    end
end

function onTriggerEnter(data)
    local tagsA = data.entityA:getTags()
    local tagsB = data.entityB:getTags()
    local scene = SceneManager:getCurrentScene()

    for _, tag in pairs(tagsA) do
        if tag == "ground" then
            if bounceCount > 2 then
                destroySelf()
            else
                local vel = body:getLinearVelocity()
                local normal = data.normal or data.contactNormal
                if normal == nil then return end

                normal = normal:Normalized()

                local reflected = vel - normal * (2.0 * vel:Dot(normal))

                -- optional homing
                local pos = transform:getWorldPosition()
                if homingDir ~= nil then
                    reflected = homingDir * reflected:Magnitude()
                end

                --print("Reflected To " + reflected)
                body:setLinearVelocity(reflected)

                print("Ball Has Bounced")
                bounceCount = bounceCount + 1
            end
        elseif tag == "player" then
            local player = scene:findEntityByName("PlayerPrefab")
            if player == nil then return end
            local message = Message.new()
            message.to = player
            message.topic = "Get Hit!"
            Script.send(message)
            SceneManager:destroy(self)
        elseif tag == "enemy" then
            if attackEnemy then
                SceneManager:destroy(data.entityA)
                destroySelf()
            end
        end
    end
    
    for _, tag in pairs(tagsB) do
        if tag == "ground" then
            if bounceCount > 2 then
                destroySelf()
            else
                local vel = body:getLinearVelocity()
                local normal = data.normal or data.contactNormal
                if normal == nil then return end

                normal = normal:Normalized()

                local reflected = vel - normal * (2.0 * vel:Dot(normal))

                -- optional homing
                local pos = transform:getWorldPosition()
                if homingDir ~= nil then
                    reflected = homingDir * reflected:Magnitude()
                end

                --print("Reflected To " + reflected)
                body:setLinearVelocity(reflected)

                print("Ball Has Bounced")
                bounceCount = bounceCount + 1
            end
        elseif tag == "player" then
            local scene = SceneManager:getCurrentScene()
            local player = scene:findEntityByName("PlayerPrefab")
            if player == nil then return end
            local message = Message.new()
            message.to = player
            message.topic = "Get Hit!"
            Script.send(message)
            SceneManager:destroy(self)
        elseif tag == "enemy" then
            if attackEnemy then
                SceneManager:destroy(data.entityB)
                destroySelf()
            end
        end
    end

    
end

function destroySelf()
    if destroyed then return end
    destroyed = true

    SceneManager:destroy(self)
end
