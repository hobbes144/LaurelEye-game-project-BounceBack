cameraTransform = nil
blasterTransform = nil
blasterEmitterTransform = nil
blasterParticleEmitter = nil
blasterSFXEmitter = nil
local canFire = true

-- Offsets relative to camera
local blasterOffsetPos = Vector3.new(1.0, -1.5, 3.0)
local blasterOffsetRot = Quaternion.new()

-- Muzzle offset relative to blaster
local muzzleOffset = Vector3.new(0.0, 1.2, 3.0)

function onStart()
    blasterTransform = self:findTransform()

    local scene = SceneManager:getCurrentScene()
    if scene ~= nil then
        local cameraEntity = scene:findEntityByName("Camera")
        if cameraEntity ~= nil then
            cameraTransform = cameraEntity:findTransform()
        end

        local blasterEmitter = scene:findEntityByName("BlasterEmitter")
        if blasterEmitter ~= nil then
            blasterEmitterTransform = blasterEmitter:findTransform()
            blasterParticleEmitter = blasterEmitter:findParticleEmitter()
            if blasterParticleEmitter then blasterParticleEmitter:pause() end
            blasterSFXEmitter = blasterEmitter:findAudioSpeaker()
        end
    end
end

function onUpdate(dt)
    updateBlasterTransform()
    updateEmitterTransform()
    local isMousePressed = Input:isMouseButtonPressed(MouseButton.Left)
    local rtAxis = Input:getGamepadAxis(GamepadAxes.RT)
    local isGamepadPressed = rtAxis > 0.5

    local pressed = isMousePressed or isGamepadPressed

    -- SEMI AUTO LOGIC
    if pressed and canFire then
        shootProjectile()
        canFire = false
    end

    -- Reset fire ability when user RELEASES the trigger/button
    if not pressed then
        canFire = true
    end
end

function updateBlasterTransform()
    local camPos = cameraTransform:getWorldPosition()
    local camRot = cameraTransform:getWorldRotation()

    local forward = camRot:forward()
    local right   = camRot:right()
    local up      = camRot:up()

    local worldPos =
        camPos +
        right   * blasterOffsetPos.x +
        up      * blasterOffsetPos.y +
        forward * blasterOffsetPos.z

    blasterTransform:setWorldPosition(worldPos)
    blasterTransform:setWorldRotation(camRot * blasterOffsetRot)
end

function updateEmitterTransform()
    if not blasterEmitterTransform then return end

    -- use same math as muzzle calculation
    local rot = blasterTransform:getWorldRotation()
    local forward = rot:forward()
    local right   = rot:right()
    local up      = rot:up()

    local basePos = blasterTransform:getWorldPosition()

    local muzzlePos =
        basePos +
        right   * muzzleOffset.x +
        up      * muzzleOffset.y +
        forward * muzzleOffset.z

    blasterEmitterTransform:setWorldPosition(muzzlePos)
    blasterEmitterTransform:setWorldRotation(rot)
end

------------------------------------------------------------
-- Compute muzzle position + direction
------------------------------------------------------------
function computeMuzzleWorldPositionAndDirection()
    local rot = blasterTransform:getWorldRotation()

    local forward = rot:forward()
    local right   = rot:right()
    local up      = rot:up()

    local basePos = blasterTransform:getWorldPosition()

    local muzzlePos =
        basePos +
        right   * muzzleOffset.x +
        up      * muzzleOffset.y +
        forward * muzzleOffset.z

    return muzzlePos, forward
end

function shootProjectile()
    local proj = SceneManager:instantiate("prefabs/blaster_projectile.prefab.json")
    if not proj then return end

    local projTransform = proj:findTransform()
    if not projTransform then return end

    local pos, dir = computeMuzzleWorldPositionAndDirection()

    projTransform:setWorldPosition(pos)
    projTransform:setWorldRotation(blasterTransform:getWorldRotation())

    local projBody = proj:findRigidBody()
    if projBody then
        projBody:setLinearVelocity(dir * 200.0)
    end

    if blasterParticleEmitter then
        blasterParticleEmitter:playFor(0.25)
    end

    if blasterSFXEmitter then
        blasterSFXEmitter:stop()
        blasterSFXEmitter:play()
    end
end
