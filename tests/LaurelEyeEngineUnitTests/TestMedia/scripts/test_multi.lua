count = 0

function onStart()
    count = count + 1
end

function onUpdate(dt)
    count = count + 1
end

function onShutdown()
    count = -999  -- sentinel
end
