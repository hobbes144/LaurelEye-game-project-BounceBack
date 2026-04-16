local pauseElements = {}
local isPaused = false
local pauseTimeScale = 0.0

local playerRef = nil

function onStart()
    local scene = SceneManager:getCurrentScene()

    pauseTimeScale = Serialized.PauseTimeScale or 0.0

    --Resolve pauseElements
    if Serialized.PauseElements ~= nil then
        for i = 1, #Serialized.PauseElements do
            local name = Serialized.PauseElements[i]

            if name ~= nil and name ~= "" then
                local ent = scene:findEntityByName(name)

                if ent ~= nil then
                    table.insert(pauseElements, ent)
                else
                    log("HUDController: Missing Pause Element -> " .. name)
                end
            end
        end
    end

    Engine.setTimeScale(1.0)

    local message = Message.new()
    message.to = playerRef
    message.topic = "Pause"
    message.contents = false
    Script.broadcast(message)
    setPauseUI(false)

    findPlayer()

end

function onMessage(msg)
    if msg.topic == "Resume Button Clicked" then
        togglePause()      
    end
end

function findPlayer()

    local scene = SceneManager:getCurrentScene()

    playerRef = scene:findEntityByName("PlayerPrefab")
    if playerRef ~= nil then
       log("HUDController: Missing Player Element ")
    end

end

function onUpdate(dt)
    if playerRef == nil then
        findPlayer()
    end

    if Input:isKeyPressed(Key.Escape) then
        togglePause()
    end
end

function togglePause()

    isPaused = not isPaused

    local message = Message.new()
    message.to = playerRef
    message.topic = "Pause"
    message.contents = isPaused
    Script.broadcast(message)

    if isPaused then
        Engine.setTimeScale(pauseTimeScale)
        setPauseUI(true)
    else
        Engine.setTimeScale(1.0)
        setPauseUI(false)
    end

end

function setPauseUI(active)

    for i = 1, #pauseElements do

        local ent = pauseElements[i]

        if ent ~= nil then
            ent:setActive(active)
        end

    end

end

