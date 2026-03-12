
--Generator script


local collider = nil

function onStart()
    collider = SceneManager:instantiate("prefabs/generator_collider.prefab.json")
    local colliderTransform = collider:findTransform()
    local selfTransform = self:findTransform()
    local selfPos =  selfTransform:getWorldPosition()
    local selfRot = selfTransform:getWorldRotation()

    local colOffset = Vector3.new(0, 8.6, 0)
    local rotOffsett = selfRot * colOffset;

    colliderTransform:setWorldPosition(selfPos + rotOffsett)

    local colliderRotOffset = Quaternion.new(-0.6235061, -0.3335269, 0.6235061, 0.3335269)

    colliderTransform:setWorldRotation(selfRot * colliderRotOffset)

                                            
end


--Continue fenerator collision situation here


--When generator is hit, texture must change to "textureBrokenScreen.png"
--Increment "broken generators" by 1.
--test if it is best to switch off collider after first hit
--regardless, no Increment after first hit, and no more than 1 hit per generator

