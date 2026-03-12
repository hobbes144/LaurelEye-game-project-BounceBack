scene = nil
doorSpawned = false

function onStart()

end

function onUpdate()
    if scene == nil then
        scene = SceneManager:getCurrentScene()
        if scene == nil then return end
        local sceneName = scene:getName()
        if sceneName == "Level1" then
            setupLevel1()
        elseif sceneName == "Level2" then
            setupLevel2()
        elseif sceneName == "Level3" then
            setupLevel3()
        elseif sceneName == "Level4" then
            setupLevel4()
        elseif sceneName == "Level5" then
            setupLevel5()
        elseif sceneName == "Level6" then
            setupLevel6()
        end
    else
        checkTargets()
    end
end

function setupLevel1()
    --Player setup
    print("STARTING SETUP FOR LEVEL 1")
    local player = SceneManager:instantiate("prefabs/catch_player.prefab.json")
    local playerTransform = player:findTransform()
    
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    --Enemy setup
    local enemy = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local enemyTransform = enemy:findTransform()
    enemyTransform:setWorldPosition(30.0, 0.0, -30.0)

    --Target setup
 --   local t1 = SceneManager:instantiate("prefabs/generator.prefab.json")
 --   local t1Transform = t1:findTransform()
 --   t1Transform:setWorldPosition(-20.0, 25.0, -48.0)

 --   local t2 = SceneManager:instantiate("prefabs/generator.prefab.json")
 --   local t2Transform = t2:findTransform()
 --   t2Transform:setWorldPosition(20.0, 25.0, -48.0)

 --   local t3 = SceneManager:instantiate("prefabs/generator.prefab.json")
 --   local t3Transform = t3:findTransform()
 --   t3Transform:setWorldPosition(0.0, 10.0, -48.0)
end

function setupLevel2()
    --Player setup
    print("STARTING SETUP FOR LEVEL 2")
    local player = SceneManager:instantiate("prefabs/catch_player.prefab.json")
    local playerTransform = player:findTransform()

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    --Enemy setup
    local e1 = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local e1Transform = e1:findTransform()
    e1Transform:setWorldPosition(0.0, 0.0, -30.0)

    local e2 = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local e2Transform = e2:findTransform()
    e2Transform:setWorldPosition(-30.0, 0.0, 10.0)

    local e3 = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local e3Transform = e3:findTransform()
    e3Transform:setWorldPosition(30.0, 0.0, 10.0)

    --Target setup
    local t1 = SceneManager:instantiate("prefabs/generator.prefab.json")
    local t1Transform = t1:findTransform()
    t1Transform:setWorldPosition(73.0, 15.0, -30.0)
    t1Transform:setWorldScale(2.0, 10.0, 20.0)

    local t2 = SceneManager:instantiate("prefabs/generator.prefab.json")
    local t2Transform = t2:findTransform()
    t2Transform:setWorldPosition(-73.0, 15.0, -30.0)
    t2Transform:setWorldScale(2.0, 10.0, 20.0)

    local t3 = SceneManager:instantiate("prefabs/generator.prefab.json")
    local t3Transform = t3:findTransform()
    t3Transform:setWorldPosition(0.0, 15.0, -48.0)
end

function setupLevel3()
    --Player setup
    print("STARTING SETUP FOR LEVEL 3")
    local player = SceneManager:instantiate("prefabs/catch_player.prefab.json")
    local playerTransform = player:findTransform()

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    --Enemy setup
    local e1 = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local e1Transform = e1:findTransform()
    e1Transform:setWorldPosition(60.0, 0.0, -40.0)

    local e2 = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local e2Transform = e2:findTransform()
    e2Transform:setWorldPosition(-60.0, 0.0, -40.0)

    local e3 = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local e3Transform = e3:findTransform()
    e3Transform:setWorldPosition(10.0, 0.0, -10.0)

    local e4 = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local e4Transform = e4:findTransform()
    e4Transform:setWorldPosition(-10.0, 0.0, -10.0)

    --Target setup
    local t1 = SceneManager:instantiate("prefabs/generator.prefab.json")
    local t1Transform = t1:findTransform()
    t1Transform:setWorldPosition(60.0, 25.0, -83.0)

    local t2 = SceneManager:instantiate("prefabs/generator.prefab.json")
    local t2Transform = t2:findTransform()
    t2Transform:setWorldPosition(-60.0, 25.0, -83.0)

    local t3 = SceneManager:instantiate("prefabs/generator.prefab.json")
    local t3Transform = t3:findTransform()
    t3Transform:setWorldPosition(0.0, 25.0, -83.0)
end

function setupLevel4()
    --TODO: Edit when merging
end

function setupLevel5()
    --TODO: Edit when merging
end

function setupLevel6()
    --TODO: Edit when merging
end

function checkTargets()
    local anyTargets = targetCheck()
    if not doorSpawned and not anyTargets then
        local scene = SceneManager:getCurrentScene()
        if scene == nil then return end
        local sceneName = scene:getName()
        if sceneName == "Level4" then
            print("Level 4 Shenanigans")
        else
            local doorSpawn = scene:findEntityByName("DoorSpawn")
            local spawnTrans = doorSpawn:findTransform()
            local doorPos = spawnTrans:getWorldPosition()

            local door = SceneManager:instantiate("prefabs/door.prefab.json")
            local doorTrans = door:findTransform()
            doorTrans:setWorldPosition(doorPos.x, doorPos.y, doorPos.z)
            doorSpawned = true
        end
    end
end

function targetCheck()
    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end

    local targetList = scene:findEntitiesWithTag("target")
    if #targetList == 0 then
        return false
    end

    return true
end
