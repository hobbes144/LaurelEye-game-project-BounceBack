function onStart()
    log("=== SERIALIZED DATA TEST BEGIN ===")

    -- Primitive tests
    log("targetScene = " .. tostring(Serialized.targetScene))
    log("testBool = " .. tostring(Serialized.testBool))
    log("testFloat = " .. tostring(Serialized.testFloat))
    log("testString = " .. tostring(Serialized.testString))

    log("=== OBJECT TEST ===")

    local enemy = Serialized.enemyConfig

    log("enemy.name = " .. tostring(enemy.name))
    log("enemy.speed = " .. tostring(enemy.speed))
    log("enemy.health = " .. tostring(enemy.health))

    log("=== NESTED OBJECT TEST ===")

    log("enemy.stats.attack = " .. tostring(enemy.stats.attack))
    log("enemy.stats.defense = " .. tostring(enemy.stats.defense))

    log("=== ARRAY TEST ===")

    local spawns = enemy.spawnPoints

    for i=1,#spawns do
        local point = spawns[i]

        log("Spawn " .. tostring(i))
        log("  x = " .. tostring(point[1]))
        log("  y = " .. tostring(point[2]))
    end

    log("=== SERIALIZED DATA TEST END ===")

    log("=== VECTOR TEST BEGIN ===")

    local v2 = Serialized.testVector2
    log("Vector2.x = " .. tostring(v2.x))
    log("Vector2.y = " .. tostring(v2.y))

    local v3 = Serialized.testVector3
    log("Vector3.x = " .. tostring(v3.x))
    log("Vector3.y = " .. tostring(v3.y))
    log("Vector3.z = " .. tostring(v3.z))

    local v4 = Serialized.testVector4
    log("Vector4.x = " .. tostring(v4.x))
    log("Vector4.y = " .. tostring(v4.y))
    log("Vector4.z = " .. tostring(v4.z))
    log("Vector4.w = " .. tostring(v4.w))

    log("=== COLOR TEST ===")

    local c = Serialized.testColor
    log("Color.r = " .. tostring(c.x))
    log("Color.g = " .. tostring(c.y))
    log("Color.b = " .. tostring(c.z))
    log("Color.a = " .. tostring(c.w))

    log("=== VECTOR TEST END ===")

    Serialized.testString = "Everybody Say AYY OOH"

end
--End OnStart()

function onHoverEnter()
    log("Button Hover Enter")
end

function onHoverExit()
    log("Button Hover Exit")
end

function onPressed( )
	log("Button Pressed")
end

function onClicked()
    log("Button Clicked")

    local sceneIndex = Serialized.targetScene
    local testBool = Serialized.testBool
    local testFloat = Serialized.testFloat
    local testString = Serialized.testString

    if sceneIndex == 1 then
        if testBool then
            for i=1,testFloat do
                log(testString)
            end

        end

        Serialized.testString = "I can Change Anything, I so Awesome"
        log(Serialized.testString)

        log("Loading Scene: Level1")
        Window.setCursorMode(0, Window.CursorMode.Disabled)
        SceneManager:changeScene("Level1")
    elseif sceneIndex == 2 then
        log("Scene Index 2")
    else
        log("BAD SCENE INDEX")
    end

    log("End Button Clicked")

end

function onReleased()
    log("Button Released")
end

function onHeld( )
	log("Button Held")
end

function onDragStart( )
	log("Button Drag Start")
end

function onDragging( )
	log("Button Dragging")
end

function onDragEnd( )
	log("Button Drag End")
end
