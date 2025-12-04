local minPlayerDistance = 45.0     -- enemies will NOT spawn closer than this
local maxPlayerDistance = 75.0    -- enemies will spawn within this max distance
local blockSpeed = 65.0

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


spawnInterval = 0.5           -- seconds between spawns
spawnTimer = 0.0               -- internal timer
spawnCount = 0


function onStart()
    math.randomseed(os.time())
end

function onUpdate(deltaTime)
    
    if spawnCount >= 20 then
        blockSpeed = blockSpeed + 5.0
        
        spawnCount = 0
    end
        
    -- accumulate time
    spawnTimer = spawnTimer + deltaTime

    if spawnTimer >= spawnInterval then
        spawnTimer = 0.0
        spawnCount = spawnCount + 1

        -- Use the Scene API exposed to Sol2
        local entity = nil
        local choices = { "Orange", "Black", "Red", "Purple" }
        local pick = choices[math.random(1, #choices)]

        if pick == "Orange" then
            entity = SceneManager:instantiate("prefabs/obstacle_orange.prefab.json")    
        elseif pick == "Black" then
            entity = SceneManager:instantiate("prefabs/obstacle_black.prefab.json")
        elseif pick == "Red" then
            entity = SceneManager:instantiate("prefabs/obstacle_red.prefab.json")
        elseif pick == "Purple" then
            entity = SceneManager:instantiate("prefabs/obstacle_purple.prefab.json")
        end

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

                -- spawn around player, keep same Y as player (or choose spawner Y)
                local spawnX = px + offsetX
                local spawnY = 3.0
                local spawnZ = pz + offsetZ
                spawnedTransform:setWorldPosition(spawnX, spawnY, spawnZ)

                -- --- Velocity calculation: set linear velocity toward the player ---
                local dirX = px - spawnX
                local dirY = spawnY
                local dirZ = pz - spawnZ

                local dist = math.sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ)
                if dist > 0.000001 then
                    dirX = dirX / dist
                    dirY = dirY / dist
                    dirZ = dirZ / dist

                    local vx = dirX * blockSpeed
                    local vy = 0.0
                    local vz = dirZ * blockSpeed

                    local phys = entity:findPhysics()
                    if phys then
                        phys:setLinearVelocity(vx, vy, vz)
                    end
                end
            end
        end
    end
end

function onShutdown()
end
