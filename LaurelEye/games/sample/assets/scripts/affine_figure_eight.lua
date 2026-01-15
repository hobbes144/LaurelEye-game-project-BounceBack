angle = 0.0
scalePhase = 0.0
speed = 1.5

transform = nil
basePos = nil
baseScale = nil

function onStart()
    print("[Affine Figure 8] Started!")
    transform = self:findTransform()  -- Cache our transform reference
    basePos = transform:getWorldPosition()
    baseScale = transform:getWorldScale()
    log(string.format("[Affine Figure 8] Base Position: %.2f, %.2f, %.2f", basePos.x, basePos.y, basePos.z))
end

function onUpdate(dt)
    if not transform then return end

    -- Increment animation time
    angle = angle + dt * speed
    scalePhase = scalePhase + dt * 2.0

    -- Figure 8 motion (lemniscate-like curve)
    local a = 5.0   -- horizontal radius
    local b = 5.0   -- depth radius
    local x = basePos.x + a * math.sin(angle)
    local z = basePos.z + b * math.sin(angle) * math.cos(angle) * 2.0

    -- Add smooth Y-axis oscillation
    local yAmplitude = 1.0
    local y = basePos.y + yAmplitude * math.sin(angle * 1.5)

    -- Apply world transform updates
    transform:setWorldPosition(Vector3.new(x, y, z))

    -- Spin continuously around Y
    local q = Quaternion.fromEuler(0.0, angle * 2.0, 0.0)
    transform:setWorldRotation(q)

    -- Subtle breathing scale effect
    local scaleFactor = 1.0 + 0.15 * math.sin(scalePhase)
    transform:setWorldScale(Vector3.new(
        baseScale.x * scaleFactor,
        baseScale.y * scaleFactor,
        baseScale.z * scaleFactor
    ))
end

function onShutdown()
    print("[AffineFigure8] Stopped after running figure-8 motion.")
end
