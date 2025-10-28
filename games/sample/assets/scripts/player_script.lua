called = false

function onStart()
    called = true
    log("LUA CODE: Lua start - called is true")
end

function onUpdate(dt)
    -- no-op for now
end

function onShutdown()
    -- no-op
    log("LUA CODE: Lua shutdown")
end
