local minPlayerDistance = 45.0     -- enemies will NOT spawn closer than this
local maxPlayerDistance = 75.0    -- enemies will spawn within this max distance

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


spawnInterval = 3.0           -- seconds between spawns
spawnTimer = 0.0               -- internal timer
spawnCount = 0                 -- how many spawned so far
totalToSpawn = 10


function onStart()
    print("[Spawner] Starting up, will spawn " .. totalToSpawn .. " enemies.")
end

function onUpdate(deltaTime)
    -- only spawn until we reach the limit
    if spawnCount >= totalToSpawn then
        return
    end

    -- accumulate time
    spawnTimer = spawnTimer + deltaTime

    if spawnTimer >= spawnInterval then
        spawnTimer = 0.0
        spawnCount = spawnCount + 1

        -- Use the Scene API exposed to Sol2
        local entity = SceneManager:instantiate("prefabs/following_enemy.prefab.json")
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


