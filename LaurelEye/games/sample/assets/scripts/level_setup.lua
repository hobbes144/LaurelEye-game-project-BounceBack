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
        elseif sceneName == "Level7" then
            setupLevel7()
        elseif sceneName == "Level8" then
            setupLevel8()
        elseif sceneName == "Level9" then
            setupLevel9()
        elseif sceneName == "BossLevel" then
            setupBossLevel()
        end
    end
end

function setupLevel1()
    --Player setup
    print("STARTING SETUP FOR LEVEL 1")

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local player = scene:findEntityByName("PlayerPrefab")
    local playerTransform = player:findTransform()
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    --Enemy setup
    local enemy = scene:findEntityByName("Enemy")
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

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local player = scene:findEntityByName("PlayerPrefab")
    local playerTransform = player:findTransform()
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    --Enemy setup
    local e1 = scene:findEntityByName("Enemy1")
    local e1Transform = e1:findTransform()
    e1Transform:setWorldPosition(56.35, 6.831, 28.497)

    local e2 = scene:findEntityByName("Enemy2")
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

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local player = scene:findEntityByName("PlayerPrefab")
    local playerTransform = player:findTransform()
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    --Enemy setup
    local e1 = scene:findEntityByName("Enemy1")
    local e1Transform = e1:findTransform()
    e1Transform:setWorldPosition(-13.225, 6.831, -40.595)

    local e2 = scene:findEntityByName("Enemy2")
    local e2Transform = e2:findTransform()
    e2Transform:setWorldPosition(55.775, 6.831, -5.06)
    local message = Message.new()
    message.to = e2
    message.topic = "I have the key!"
    Script.send(message)
end

function setupLevel4()
    print("STARTING SETUP FOR LEVEL 4")

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local player = scene:findEntityByName("PlayerPrefab")
    local playerTransform = player:findTransform()
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    local e1 = scene:findEntityByName("Enemy1")
    local e1Transform = e1:findTransform()
    e1Transform:setWorldPosition(43.01, 6.831, -14.145)

    local e2 = scene:findEntityByName("Enemy2")
    local e2Transform = e2:findTransform()
    e2Transform:setWorldPosition(31.625, 6.831, 2.185)

    local e3 = scene:findEntityByName("Enemy3")
    local e3Transform = e3:findTransform()
    e3Transform:setWorldPosition(-25.806, 6.831, 50.7955)
    local message = Message.new()
    message.to = e3
    message.topic = "I have the key!"
    Script.send(message)
end

function setupLevel5()
    print("STARTING SETUP FOR LEVEL 5")

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local player = scene:findEntityByName("PlayerPrefab")
    local playerTransform = player:findTransform()
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    local e1 = scene:findEntityByName("SecurityEnemy")
    local e1Transform = e1:findTransform()
    e1Transform:setWorldPosition(35.42, 6.831, -113.045)
    local message = Message.new()
    message.to = e1
    message.topic = "I have the key!"
    Script.send(message)
end

function setupLevel6()
    print("STARTING SETUP FOR LEVEL 6")

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local player = scene:findEntityByName("PlayerPrefab")
    local playerTransform = player:findTransform()
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    --Enemy setup
    local e1 = scene:findEntityByName("Enemy1")
    local e1Transform = e1:findTransform()
    e1Transform:setWorldPosition(-295.205, 9.89, -19.205)

    local e2 = scene:findEntityByName("Enemy2")
    local e2Transform = e2:findTransform()
    e2Transform:setWorldPosition(-423.775, 9.89, -87.055)

    local e3 = scene:findEntityByName("Enemy3")
    local e3Transform = e3:findTransform()
    e3Transform:setWorldPosition(-444.705, 9.89, -74.86501)
    local message2 = Message.new()
    message2.to = e3
    message2.topic = "I have the key!"
    Script.send(message2)
end

function setupLevel7()
    print("STARTING SETUP FOR LEVEL 7")

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local player = scene:findEntityByName("PlayerPrefab")
    local playerTransform = player:findTransform()
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    --Enemy setup
    local e1 = scene:findEntityByName("Enemy1")
    local e1Transform = e1:findTransform()
    e1Transform:setWorldPosition(-423.775, 30, -87.055)

    local e2 = scene:findEntityByName("Enemy2")
    local e2Transform = e2:findTransform()
    e2Transform:setWorldPosition(-297.85, 30, 28.405)

    local e3 = scene:findEntityByName("Enemy3")
    local e3Transform = e3:findTransform()
    e3Transform:setWorldPosition(-444.705, 30, -74.86501 )
    local message2 = Message.new()
    message2.to = e3
    message2.topic = "I have the key!"
    Script.send(message2)
end


function setupLevel8()
    --TODO: Edit when merging
    print("STARTING SETUP FOR LEVEL 8")

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local player = scene:findEntityByName("PlayerPrefab")
    local playerTransform = player:findTransform()
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    --Enemy setup
    local e1 = scene:findEntityByName("Enemy1")
    local e1Transform = e1:findTransform()
    e1Transform:setWorldPosition(-383.065, 8.625, 50.715)

    local e2 = scene:findEntityByName("Enemy2")
    local e2Transform = e2:findTransform()
    e2Transform:setWorldPosition(-424.81, 8.625, -34.293)
    local message = Message.new()
    message.to = e2
    message.topic = "I have the key!"
    Script.send(message)

    -- local e3 = scene:findEntityByName("Enemy3")
    -- local e3Transform = e3:findTransform()
    -- e3Transform:setWorldPosition(-108.1, 7.015, 29.785 )
    -- local message2 = Message.new()
    -- message2.to = e3
    -- message2.topic = "I have the key!"
    -- Script.send(message2)

    local e4 = scene:findEntityByName("Enemy4")
    local e4Transform = e4:findTransform()
    e4Transform:setWorldPosition(-200.1, 7.015, -65.205)

    -- local e5 = scene:findEntityByName("Enemy5")
    -- local e5Transform = e5:findTransform()
    -- e5Transform:setWorldPosition(-269.905, 27.6, -71.99001)

    local e6 = scene:findEntityByName("Enemy6")
    local e6Transform = e6:findTransform()
    e6Transform:setWorldPosition( -182.735, 7.015, -77.165)

    local e7 = scene:findEntityByName("Enemy7")
    local e7Transform = e7:findTransform()
    e7Transform:setWorldPosition( -261.51, 27.6, -43.7)

end

function setupLevel9()
    --TODO: Edit when merging
    print("STARTING SETUP FOR LEVEL 9")

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local player = scene:findEntityByName("PlayerPrefab")
    local playerTransform = player:findTransform()
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

   -- local e1 = SceneManager:instantiate("prefabs/security_enemy.prefab.json")
   -- local e1Transform = e1:findTransform()
   -- e1Transform:setWorldPosition(35.42, 6.831, -113.045)
end

function setupBossLevel()
    print("STARTING SETUP FOR BOSS LEVEL")

    local scene = SceneManager:getCurrentScene()
    if scene == nil then return end
    local player = scene:findEntityByName("PlayerPrefab")
    local playerTransform = player:findTransform()
    local spawnPoint = scene:findEntityByName("SpawnPoint")
    local spawnTrans = spawnPoint:findTransform()
    local spawnPos = spawnTrans:getWorldPosition()

    playerTransform:setWorldPosition(spawnPos.x, spawnPos.y, spawnPos.z)

    local camera = SceneManager:instantiate("prefabs/tpcamera.prefab.json")

    -- local e1 = SceneManager:instantiate("prefabs/boss_enemy.prefab.json")
    -- local e1Transform = e1:findTransform()
    -- e1Transform:setWorldPosition(35.42, 6.831, -153.045)
    --
    -- local e2 = SceneManager:instantiate("prefabs/boss_enemy2.prefab.json")
end
