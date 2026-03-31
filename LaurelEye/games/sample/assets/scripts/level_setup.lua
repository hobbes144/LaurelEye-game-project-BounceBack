scene = nil
doorSpawned = false

function onStart()

end

function onUpdate()
    if scene == nil then
        scene = SceneManager:getCurrentScene()
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

    local message = Message.new()
    message.to = enemy
    message.topic = "I have the key!"
    Script.send(message)
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
    e1Transform:setWorldPosition(56.35, 6.831, 28.497)

    local e2 = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local e2Transform = e2:findTransform()
    e2Transform:setWorldPosition(39.445, 6.831, 43.24)
    local message = Message.new()
    message.to = e2
    message.topic = "I have the key!"
    Script.send(message)
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
    e1Transform:setWorldPosition(-13.225, 6.831, -40.595)

    local e2 = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local e2Transform = e2:findTransform()
    e2Transform:setWorldPosition(55.775, 6.831, -5.06)
    local message = Message.new()
    message.to = e2
    message.topic = "I have the key!"
    Script.send(message)
end

function setupLevel4()
    print("STARTING SETUP FOR LEVEL 4")
    local player = SceneManager:instantiate("prefabs/catch_player.prefab.json")
    local playerTransform = player:findTransform()

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    local e1 = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local e1Transform = e1:findTransform()
    e1Transform:setWorldPosition(43.01, 6.831, -14.145)

    local e2 = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local e2Transform = e2:findTransform()
    e2Transform:setWorldPosition(31.625, 6.831, 2.185)

    local e3 = SceneManager:instantiate("prefabs/shooting_enemy.prefab.json")
    local e3Transform = e3:findTransform()
    e3Transform:setWorldPosition(-25.806, 6.831, 50.7955)
    local message = Message.new()
    message.to = e3
    message.topic = "I have the key!"
    Script.send(message)
end

function setupLevel5()
    print("STARTING SETUP FOR LEVEL 5")
    local player = SceneManager:instantiate("prefabs/catch_player.prefab.json")
    local playerTransform = player:findTransform()

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    local e1 = SceneManager:instantiate("prefabs/security_enemy.prefab.json")
    local e1Transform = e1:findTransform()
    e1Transform:setWorldPosition(35.42, 6.831, -113.045)
end

function setupLevel6()
    --TODO: Edit when merging
end
