local EPS = 1e-6

--- Reflect a Vector3
---@param dir Vector3
---@param normal Vector3
---@return Vector3
local function reflect(dir, normal)
    -- assumes normal is normalized; dir should be normalized too
    local d = dir:Dot(normal)
    return dir - (normal * 2.0 * d)
end

-- Builds a quaternion that rotates +Z to targetDir
---@param targetDir Vector3
---@return Quaternion
local function lookRotationFromZ(targetDir)
    local dir = targetDir:Normalized()

    local eye = Vector3.new(0, 0, 0)
    local center = dir
    local up = Vector3.new(0, 1, 0)

    -- Handle near-parallel forward/up case
    if math.abs(dir:Dot(up)) > 1.0 - EPS then
        up = Vector3.new(1, 0, 0)
    end

    local m = Matrix4.lookAt(eye, center, up)
    return m:toQuaternion()
end

--- Compute acceleration from gravity force and mass
---@param gravityForce Vector3
---@param mass number
---@return Vector3
local function getGravityAcceleration(gravityForce, mass)
    if mass == nil or math.abs(mass) < EPS then
        return Vector3.new(0, 0, 0)
    end
    return gravityForce / mass
end

--- Simulate projectile arc and find first hit
---@param startPos Vector3
---@param initialVel Vector3
---@param accel Vector3
---@param maxTime number
---@param dt number
---@param opt RaycastOpt
---@return Vector3|nil hitPos
---@return Vector3|nil hitNormal
---@return Vector3|nil hitVelocity
---@return number chordDistance
local function simulateArcHit(startPos, initialVel, accel, maxTime, dt, opt)
    local pos = startPos
    local vel = initialVel
    local elapsed = 0.0

    while elapsed < maxTime do
        local step = math.min(dt, maxTime - elapsed)

        local nextPos = pos + (vel * step) + (accel * (0.5 * step * step))
        local move = nextPos - pos
        local segLen = move:Magnitude()

        if segLen > EPS then
            local dir = move / segLen
            local hit = Physics.Raycast(pos, dir, segLen, opt)

            if hit ~= nil and hit.position ~= nil then
                local impactVel = vel + accel * step
                return hit.position, hit.normal, impactVel, (hit.position - startPos):Magnitude()
            end
        end

        pos = nextPos
        vel = vel + accel * step
        elapsed = elapsed + step
    end

    return nil, nil, nil, 0.0
end

--- Get the bounce point and direction
---@param playerPos Vector3
---@param playerAimDir Vector3
---@param speed number
---@param gravityForce Vector3
---@param mass number
---@param maxTime number
---@param dt number
---@return Vector3|nil position, Vector3|nil direction, number distance, Vector3|nil velocity
local function getGravityBounce(playerPos, playerAimDir, speed, gravityForce, mass, maxTime, dt)

    ---@type RaycastOpt
    local opt = { layerMask = (~(Physics.Layers.Player | Physics.Layers.Projectile)) & Physics.Layers.All }
    ---@type Vector3|nil
    local bouncePos = nil
    ---@type Vector3|nil
    local bounceDir = nil
    ---@type number
    local bounceDist = 0.0
    ---@type Vector3|nil
    local hitNormal = nil
    ---@type Vector3|nil
    local hitVelocity = nil

    local accel = getGravityAcceleration(gravityForce, mass)
    local initialVel = playerAimDir:Normalized() * speed

    bouncePos, hitNormal, hitVelocity, bounceDist =
        simulateArcHit(playerPos, initialVel, accel, maxTime, dt, opt)

    if bouncePos ~= nil and hitNormal ~= nil and hitVelocity ~= nil then
        hitNormal = hitNormal:Normalized()

        if hitVelocity:Dot(hitNormal) > 0 then
            hitNormal = hitNormal * -1.0
        end

        bounceDir = reflect(hitVelocity:Normalized(), hitNormal):Normalized()
    end

    return bouncePos, bounceDir, bounceDist, hitVelocity
end

--- Get the bounce point and direction
---@param playerPos Vector3
---@param playerAimDir Vector3
---@param maxRange number
---@return Vector3|nil, Vector3|nil, number
local function getBounce(playerPos, playerAimDir, maxRange)

    ---@type RaycastOpt
    local opt = { layerMask = (~(Physics.Layers.Player | Physics.Layers.Projectile)) & Physics.Layers.All }
    ---@type Vector3|nil
    local bouncePos = nil
    ---@type Vector3|nil
    local bounceDir = nil
    ---@type number
    local bounceDist = 0.0

    -- Cast forward
    local hit = Physics.Raycast(playerPos, playerAimDir, maxRange, opt)
    if hit ~= nil then
        bouncePos = hit.position
        bounceDist = hit.distance
        if hit.normal ~= nil then
            hit.normal = hit.normal:Normalized()
            -- If the normal faces the wrong way, reverse it
            if playerAimDir:Dot(hit.normal) > 0 then
                hit.normal = hit.normal * -1.0
            end
            bounceDir = reflect(playerAimDir, hit.normal):Normalized()
        end
    end

    return bouncePos, bounceDir, bounceDist
end

--- Smart ball targeting
---@param origin Vector3|nil
---@param baseDir Vector3|nil
---@return Vector3|nil, RaycastResult|nil
function findEnemyDirection(origin, baseDir)
    local maxDistance = 75.0
    local coneAngle = math.rad(15)
    local rings = 3
    local raysPerRing = 6

    baseDir = baseDir:Normalized()

    local up = Vector3.new(0, 1, 0)
    if math.abs(baseDir:Dot(up)) > 0.95 then
        up = Vector3.new(1, 0, 0)
    end

    local right = baseDir:Cross(up):Normalized()
    up = right:Cross(baseDir):Normalized()

    for r = 1, rings do
        local t = r / rings
        local angle = coneAngle * t

        for i = 1, raysPerRing do
            local theta = (i / raysPerRing) * math.pi * 2.0

            local offset =
                right * math.cos(theta) * math.sin(angle) +
                up    * math.sin(theta) * math.sin(angle)

            local dir = (baseDir * math.cos(angle) + offset):Normalized()

            local hit = Physics.Raycast(origin, dir, maxDistance, { layerMask = Layers.Enemy })

            if hit then
                return dir, hit
            end
        end
    end

    return nil, nil
end


---@class TrajectoryLine
---@field aimLine Entity
---@field aimLineTransform TransformComponent
---@field aimLineWallOffset number
---@field aimLineLengthMult number
---@field bounceLine Entity
---@field bounceLineTransform TransformComponent
---@field bounceLineWallOffset number
---@field bounceLineLengthMult number
---@field lineWidth number
---@field offScreenPos Vector3
---@field simDt number
---@field simMaxTime number
---@field bounceRestitution number
local TrajectoryLine = {}
TrajectoryLine.__index = TrajectoryLine

function TrajectoryLine.new()
    local self = setmetatable({}, TrajectoryLine)

    self.aimLine = SceneManager:instantiate("prefabs/trajectory-line.prefab.json")
    self.aimLineTransform = self.aimLine:findTransform()
    self.aimLineWallOffset = 5.0
    self.aimLineLengthMult = 0.3
    self.bounceLine = SceneManager:instantiate("prefabs/trajectory-line.prefab.json")
    self.bounceLineTransform = self.bounceLine:findTransform()
    self.bounceLineWallOffset = 0.0
    self.bounceLineLengthMult = 0.2
    self.lineWidth = 0.3
    self.offScreenPos = Vector3.new(0.0, -100.0, 0.0)

    -- Gravity simulation settings
    self.simDt = 0.1
    self.simMaxTime = 3.0
    self.bounceRestitution = 1.0 -- lower this if your real projectile loses speed on bounce

    return self
end

--- Update a line using start and end positions directly
---@param transform TransformComponent
---@param startPos Vector3
---@param endPos Vector3
---@param offset number
function TrajectoryLine:_updateLineFromPoints(transform, startPos, endPos, offset)
    local chord = endPos - startPos
    local distance = chord:Magnitude()

    if distance < EPS then
        transform:setWorldPosition(self.offScreenPos)
        return
    end

    local dir = chord / distance
    local lineLength = math.max(distance - offset, EPS)
    local centerPos = startPos + dir * (0.5 * lineLength + offset)

    transform:setWorldPosition(centerPos)
    transform:setWorldRotation(lookRotationFromZ(dir))
    transform:setWorldScale(Vector3.new(self.lineWidth, self.lineWidth, lineLength))
end

--- Update trajectory lines
---@param transform TransformComponent
---@param bouncePos Vector3
---@param direction Vector3
---@param distance number
---@param lengthMul number
---@param offset number
function TrajectoryLine:_updateLine(transform, bouncePos, direction, distance, lengthMul, offset)

    ---@type number
    local lineLength = lengthMul * distance
    if distance < (2 * offset) then
        lineLength = 0.5 * distance
    end

    ---@type number
    local linePosOffset = (0.5 * lineLength) + offset
    -- Set the line from player to the bounce point
    transform:setWorldPosition(
        bouncePos - (direction * linePosOffset))
    transform:setWorldRotation(lookRotationFromZ(direction))
    transform:setWorldScale(Vector3.new(self.lineWidth, self.lineWidth, lineLength))
end

--- Message callback to receive ball interaction messages
---@param message Message
function TrajectoryLine:ballCallback(message)
    if message.topic == "FirstBounce" then
        self.aimLineTransform:setWorldPosition(self.offScreenPos)
        self.bounceLineTransform:setWorldPosition(self.offScreenPos)
    end
end

function TrajectoryLine:resetLine()
    self.aimLineTransform:setWorldPosition(self.offScreenPos)
    self.bounceLineTransform:setWorldPosition(self.offScreenPos)
end

--- Update trajectory lines with rough gravity
---@param playerPos Vector3
---@param playerAimDir Vector3
---@param projectileSpeed number
---@param projectileMass number
---@param gravityForce Vector3
function TrajectoryLine:updateTrajectory(playerPos, playerAimDir, projectileSpeed, projectileMass, gravityForce)
    playerAimDir = playerAimDir:Normalized()

    local accel = getGravityAcceleration(gravityForce, projectileMass)

    -- Check if ball can bounce off an object in range
    ---@type Vector3|nil
    local bouncePos
    ---@type Vector3|nil
    local bounceDir
    ---@type number
    local bounceDist
    ---@type Vector3|nil
    local hitVelocity

    bouncePos, bounceDir, bounceDist, hitVelocity =
        getGravityBounce(
            playerPos,
            playerAimDir,
            projectileSpeed,
            gravityForce,
            projectileMass,
            self.simMaxTime,
            self.simDt
        )

    if bouncePos == nil or bounceDist <= EPS then
        self:resetLine()
        return
    end

    self:_updateLineFromPoints(
        self.aimLineTransform,
        playerPos,
        bouncePos,
        self.aimLineWallOffset
    )

    if bounceDir == nil or hitVelocity == nil then
    -- if bounceDir == nil then
        self.bounceLineTransform:setWorldPosition(self.offScreenPos)
        return
    end

    -- -- Use reflected speed after impact
    -- local bouncedSpeed = hitVelocity:Magnitude() * self.bounceRestitution
    -- local bouncedVel = bounceDir * bouncedSpeed
    --
    -- -- Small offset to avoid immediate self-hit on the same wall
    -- local secondStart = bouncePos + bounceDir * 0.05
    --
    -- local opt = { layerMask = (~(Physics.Layers.Player | Physics.Layers.Projectile)) & Physics.Layers.All }
    -- local secondPos = nil
    --
    -- -- Optional enemy assist: keep your existing straight cone search
    -- local enemyDir, enemyHit = findEnemyDirection(secondStart, bounceDir)
    -- if enemyHit ~= nil and enemyHit.position ~= nil then
    --     secondPos = enemyHit.position
    -- else
    --     local wallPos = nil
    --     wallPos = select(1, simulateArcHit(secondStart, bouncedVel, accel, self.simMaxTime, self.simDt, opt))
    --     secondPos = wallPos
    -- end
    --
    -- if secondPos ~= nil then
    --     self:_updateLineFromPoints(
    --         self.bounceLineTransform,
    --         bouncePos,
    --         secondPos,
    --         self.bounceLineWallOffset
    --     )
    -- else
    --     self.bounceLineTransform:setWorldPosition(self.offScreenPos)
    -- end

    ---@type Vector3|nil
    local enemyDir
    ---@type RaycastResult|nil
    local enemyHit
    enemyDir, enemyHit = findEnemyDirection(bouncePos, bounceDir)

    if enemyHit ~= nil and enemyHit.distance > EPS and enemyHit.position ~= nil and enemyDir ~= nil then
        self:_updateLine(
            self.bounceLineTransform,
            bouncePos, -enemyDir, enemyHit.distance,
            self.bounceLineLengthMult, self.bounceLineWallOffset)
    else

        ---@type Vector3|nil
        local wallPos
        ---@type Vector3|nil
        local wallDir
        ---@type number
        local wallDist
        wallPos, wallDir, wallDist = getBounce(bouncePos, bounceDir, 1000.0)
        if wallDist > EPS then
            self:_updateLine(
                self.bounceLineTransform,
                bouncePos, -bounceDir, wallDist,
                self.bounceLineLengthMult, self.bounceLineWallOffset)
        else
            self.bounceLineTransform:setWorldPosition(self.offScreenPos)
        end
    end
end

return TrajectoryLine
