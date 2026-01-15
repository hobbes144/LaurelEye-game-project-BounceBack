local minPlayerDistance = 50.0     -- enemies will NOT spawn closer than this
local maxPlayerDistance = 100.0    -- enemies will spawn within this max distance

local function randomPointOutsideRadius(minRadius, maxRadius)
    -- random angle around circle
    local angle = math.random() * math.pi * 2

    -- random distance between min and max
    local r = minRadius + math.random() * (maxRadius - minRadius)

    -- convert polar → cartesian
    local offsetX = math.cos(angle) * r
    local offsetZ = math.sin(angle) * r

    return offsetX, offsetZ
end


spawnInterval = 2.0           -- seconds between spawns
spawnTimer = 0.0               -- internal timer
spawnCount = 0                 -- how many spawned so far

function onStart()

end

function onUpdate(deltaTime)
    -- accumulate time
    spawnTimer = spawnTimer + deltaTime
    if spawnCount == 10 and spawnInterval > 0.0 then
        spawnInterval = spawnInterval - 0.2
        spawnCount = 0
    end
    if spawnTimer >= spawnInterval then
        spawnTimer = 0.0

        -- Use the Scene API exposed to Sol2
        local entity = SceneManager:instantiate("prefabs/zombie.prefab.json")
        print("[Spawner] Spawned Enemy #" .. tostring(spawnCount))

        local scene = SceneManager:getCurrentScene()

        if entity ~= nil and self ~= nil then
            local spawnedTransform = entity:findTransform()
            local player = scene:findEntityByName("PlayerPrefab")

            if spawnedTransform and player then
                local playerTransform = player:findTransform()
                local playerPos = playerTransform:getWorldPosition()
                local px = playerPos.x
                local py = playerPos.y
                local pz = playerPos.z

                -- calculate random offset
                local offsetX, offsetZ = randomPointOutsideRadius(minPlayerDistance, maxPlayerDistance)

                -- spawn around player, keep same Y as spawner (or use py)
                spawnedTransform:setWorldPosition(
                    px + offsetX,
                    py,     -- choose Y source
                    pz + offsetZ
                )
            end
        end
    end
end

function onShutdown()
    print("[Spawner] Shutting down after spawning " .. tostring(spawnCount) .. " enemies.")
end


