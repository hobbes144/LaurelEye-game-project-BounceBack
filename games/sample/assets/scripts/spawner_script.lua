spawnInterval = 0.05           -- seconds between spawns
spawnTimer = 0.0               -- internal timer
spawnCount = 0                 -- how many spawned so far
totalToSpawn = 100
spawnRadius = 3.0


function onStart()
    print("[Spawner] Starting up, will spawn " .. totalToSpawn .. " physics balls.")
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
        local entity = SceneManager:instantiate("prefabs/physics_ball.prefab.json")
        print("[Spawner] Spawned ball #" .. tostring(spawnCount))

        if entity ~= nil and self ~= nil then
            local spawnerTransform = self:findTransform()
            local spawnedTransform = entity:findTransform()
            if spawnerTransform ~= nil and spawnedTransform ~= nil then
                local pos = spawnerTransform:getWorldPosition()
                local offsetX = (math.random() - 0.5) * 2.0 * spawnRadius
                local offsetY = (math.random() - 0.5) * 2.0 * spawnRadius
                local offsetZ = (math.random() - 0.5) * 2.0 * spawnRadius
                spawnedTransform:setWorldPosition(pos.x + offsetX, pos.y + offsetY, pos.z + offsetZ)
            end
        end
    end
end

function onShutdown()
    print("[Spawner] Shutting down after spawning " .. tostring(spawnCount) .. " balls.")
end
