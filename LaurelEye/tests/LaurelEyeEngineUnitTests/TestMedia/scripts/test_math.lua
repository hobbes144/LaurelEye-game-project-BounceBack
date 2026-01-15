vAddX, vAddY, vAddZ = 0, 0, 0
qRotX, qRotY, qRotZ = 0, 0, 0
mDirX, mDirY, mDirZ = 0, 0, 0
t3PosX, t3PosY, t3PosZ = 0, 0, 0
t3DirX, t3DirY, t3DirZ = 0, 0, 0

function vectorTest()
    local v1 = Vector3.new(1, 2, 3)
    local v2 = Vector3.new(4, 5, 6)
    local sum = v1 + v2
    vAddX = sum.x
    vAddY = sum.y
    vAddZ = sum.z
end

function quaternionTest()
    local axis = Vector3.new(0, 1, 0)
    local q = Quaternion.axisAngleToQuaternion(axis, math.pi / 2)
    local v = Vector3.new(0, 0, 1)
    local rotated = q * v
    qRotX = rotated.x
    qRotY = rotated.y
    qRotZ = rotated.z
end

function matrixTest()
    local trans = Matrix4.translation(Vector3.new(1, 2, 3))
    local dir = Vector3.new(1, 0, 0)
    local transformedDir = trans:transformDirection(dir)
    mDirX = transformedDir.x
    mDirY = transformedDir.y
    mDirZ = transformedDir.z
end

function transformTest()
    local t1 = Transform.new()
    t1:setPosition(1, 0, 0)
    t1:setRotation(0, math.pi / 2, 0) -- 90° around Y
    t1:setScaling(2, 2, 2)

    local t2 = Transform.new()
    t2:setPosition(0, 0, 1)

    local t3 = t1 * t2

    local t3Pos = t3:getPosition()
    t3PosX = t3Pos.x
    t3PosY = t3Pos.y
    t3PosZ = t3Pos.z

    local t3Mat = t3:getLocalMatrix()
    local dir = Vector3.new(0, 0, 1)
    local transformedDir = t3Mat:transformDirection(dir)
    t3DirX = transformedDir.x
    t3DirY = transformedDir.y
    t3DirZ = transformedDir.z
end

function onStart()
    log("[LUA] Running math test...")

    -- ==========================
    -- Vector3 test
    -- ==========================
   vectorTest()

    -- ==========================
    -- Quaternion test
    -- Rotate (0,0,1) by 90° around Y
    -- ==========================
    quaternionTest()

    -- ==========================
    -- Matrix test
    -- Translate then transform a direction
    -- ==========================
    matrixTest()

    -- ==========================
    -- Transform test
    -- ==========================
    transformTest()
end
