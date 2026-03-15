
--Generator script


local collider = nil
brokenMat = nil

function onStart()

    collider = SceneManager:instantiate("prefabs/generator_collider.prefab.json")

    print("Collider: ", collider)
    local message = Message.new()
    message.to = collider
    message.from = self
    message.topic = "Tis I!"
    Script.send(message)


    local colliderTransform = collider:findTransform()
    local selfTransform = self:findTransform()

    local selfPos = selfTransform:getWorldPosition()
    local selfRot = selfTransform:getWorldRotation()

    local colOffset = Vector3.new(0, 8.6, 0)
    local rotOffset = selfRot * colOffset

    colliderTransform:setWorldPosition(selfPos + rotOffset)

    local colliderRotOffset = Quaternion.new(-0.6235061, -0.3335269, 0.6235061, 0.3335269)
    colliderTransform:setWorldRotation(selfRot * colliderRotOffset)

    local scene = SceneManager:getCurrentScene()
    local brokenGen = scene:findEntityByName("brokenGen")

    local brokenRender = brokenGen:findComponent("Renderable3DComponent")

    brokenMat = brokenRender:getMaterial()

end

function onMessage(msg)
    if msg.topic == "Change Texture!" then
        print("I am getting destroyed")
        local renderComp = self:findComponent("Renderable3DComponent")

        renderComp:setMaterial(brokenMat)
    end

end

--Continue fenerator collision situation here


--When generator is hit, texture must change to "textureBrokenScreen.png"
--Increment "broken generators" by 1.
--test if it is best to switch off collider after first hit
--regardless, no Increment after first hit, and no more than 1 hit per generator

