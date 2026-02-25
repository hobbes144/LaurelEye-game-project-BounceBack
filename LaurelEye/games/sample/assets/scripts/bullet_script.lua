lifeTime = 3.0          -- total lifespan before auto-destruction
destroyed = false

timeAlive = 0.0
transform = nil
body = nil

speaker = nil

function onStart()
    transform = self:findTransform()
    body = self:findGhostBody()

    speaker = self:findAudioSpeaker()
    if speaker ~= nil then
        speaker:stop()
        speaker:play()
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
    for _, tag in pairs(tagsA) do
        if tag == "ground" then
            destroySelf()
        elseif tag == "player" then
            local scene = SceneManager:getCurrentScene()
            local player = scene:findEntityByName("PlayerPrefab")
            if player == nil then return end
            local message = Message.new()
            message.to = player
            message.topic = "Get Hit!"
            Script.send(message)
            SceneManager:destroy(self)
        end
    end

    local tagsB = data.entityB:getTags()
    for _, tag in pairs(tagsB) do
        if tag == "ground" then
            destroySelf()
        elseif tag == "player" then
            local scene = SceneManager:getCurrentScene()
            local player = scene:findEntityByName("PlayerPrefab")
            if player == nil then return end
            local message = Message.new()
            message.to = player
            message.topic = "Get Hit!"
            Script.send(message)
            SceneManager:destroy(self)
        end
    end

    
end

function destroySelf()
    if destroyed then return end
    destroyed = true

    SceneManager:destroy(self)
end
