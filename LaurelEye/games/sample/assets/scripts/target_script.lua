destroyed = false
transform = nil
body = nil

function onStart()
    transform = self:findTransform()
    body = self:findPhysics()
end

function onUpdate(dt)
    if destroyed then return end
end

function onCollisionEnter(data)
    local tagsA = data.entityA:getTags()
    for _, tag in pairs(tagsA) do
        if tag == "ball" then
            destroySelf()
        end

    end

    local tagsB = data.entityB:getTags()
    for _, tag in pairs(tagsB) do
        if tag == "ball" then
            destroySelf()
        end
    end
end

function destroySelf()
    if destroyed then return end
    destroyed = true

    SceneManager:destroy(self)
end
