function onStart()
    log("Scene Test: onStart")

    -- Get current scene
    local scene = SceneManager:getCurrentScene()
    if scene then
        log("Current scene: " .. scene:getName())

        -- List entities in the scene
        local entities = scene:getEntities()
        if entities then
            log("Entity count: " .. tostring(#entities))
        else
            log("No entities returned from getEntities()")
        end

        -- Try find an entity by name
        local e = scene:findEntityByName("Player")
        if e then
            log("Found entity 'Player': " .. e:getName())
        else
            log("Entity 'Player' not found")
        end
    else
        log("No current scene found")
    end
end

function onUpdate(dt)
    -- For testing, change scenes after a bit
    if dt > 0 then
        log("Reloading current scene...")
        SceneManager:reloadCurrentScene()
    end
end
