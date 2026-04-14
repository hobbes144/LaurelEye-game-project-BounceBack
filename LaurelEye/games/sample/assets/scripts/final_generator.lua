local health = 5
local riseSpeed = 2          -- units per second
local riseDistance = 3       -- how far it rises
local startY                 -- starting Y position (below floor)
local targetY                -- final resting Y position
local rising = false
local riseProgress = 0       -- 0.0 to 1.0
local transform = nil

local sparkPE = nil
local smokePE = nil

function onStart()
    -- Grab current position and set up rise targets
    transform = self:findTransform()
    local pos = transform:getWorldPosition()
    targetY = -0.08
    startY = pos.y

    local scene = SceneManager:getCurrentScene()
    local spark = scene:findEntityByName("SparkEmitter")
    sparkPE = spark and spark:findParticleEmitter()
    print("SparkPE: ", sparkPE)
    sparkPE:pause()

    local smoke = scene:findEntityByName("SmokeEmitter")
    smokePE = smoke and smoke:findParticleEmitter()
    print("SmokePE: ", smokePE)
    smokePE:pause()

    rising = true
end

-- Smooth ease-out interpolation
local function easeOut(t)
    return 1 - (1 - t) * (1 - t)
end

function onUpdate(dt)
    if rising then
        riseProgress = riseProgress + (dt * riseSpeed / riseDistance)

        if riseProgress >= 1.0 then
            riseProgress = 1.0
            rising = false
        end

        local easedT = easeOut(riseProgress)
        local pos = transform:getWorldPosition()
        local newY = startY + (targetY - startY) * easedT
        transform:setWorldPosition(pos.x, newY, pos.z)
    end
end

function onMessage(msg)
    if msg.topic == "Get Destroyed!" then
        health = health - 1
        print("Final Generator Health Remaining: ", health)
        sparkPE:playFor(0.2)
        if health <= 0 then
            local scene = SceneManager:getCurrentScene()
            local boss = scene:findEntityByName("BossEnemy")
            smokePE:play()
            local message = Message.new()
            message.to = boss
            message.topic = "Avenge Me!"
            Script.send(message)
        end
    end
end
