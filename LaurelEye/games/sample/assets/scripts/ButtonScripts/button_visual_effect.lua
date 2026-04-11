--Button Menu Visuals

local renderUI
local material
local transformUI
local interactionUI

local normalTex
local hoverTex
local pressedTex

local hoverEnt
local pressedEnt

--Compression

local baseSize
local holdTime = 0

-- Tuning values
local pressSquish = 0.95
local holdAmplitude = 1.5
local holdSpeed = 20.0

function onStart()
    local scene = SceneManager:getCurrentScene()

    -- Get components using YOUR bindings
    render = self:findUIRender()
    transform = self:findUITransform()
    interaction = self:findUIInteraction()

    -- Pull serialized values
    local hoverEntNameStr = Serialized.hoverEntName
    local pressedEntNameStr = Serialized.pressedEntName

    if hoverEntNameStr ~= nil then
        hoverEnt = scene:findEntityByName(hoverEntNameStr)
    end

    if pressedEntNameStr ~= nil then 
        pressedEnt = scene:findEntityByName(pressedEntNameStr)
    end

    if hoverEnt == nil then
        log("ERROR: No Hover Entity found")
    end

    if pressedEnt == nil then
        log("ERROR: No Pressed Entity found")
    end

    if hoverEnt ~= nil then
        local hoverUIRender = hoverEnt:findUIRender()
        if hoverUIRender ~= nil then
            local hoverMat = hoverUIRender:getMaterial()
            if hoverMat ~= nil then
                hoverTex = hoverMat:getTexture("mainTexture")
                log("Hover Tex: " .. hoverTex)
            end
        end
    end

    if pressedEnt ~= nil then
        local pressedUIRender = pressedEnt:findUIRender()
        if pressedUIRender ~= nil then
            local pressedMat = pressedUIRender:getMaterial()
            if pressedMat ~= nil then
                pressedTex = pressedMat:getTexture("mainTexture")
                log("Pressed Tex: " .. pressedTex)
            end
        end
    end



    if render == nil then
        log("ERROR: No UIRenderComponent found")
    end

    if render ~= nil then
        material = render:getMaterial()
        if material ~= nil then
            normalTex = material:getTexture("mainTexture")
        end
    end

    if transform ~= nil then
        baseSize = transform:getSize()
    end

end

function onHoverEnter()
    log("Hover Visual Enter")

    if render ~= nil and hoverTex ~= nil then
        log("Changing Texture")
        material:setTexture("mainTexture", hoverTex)
    end
end

function onHoverExit()
    if render ~= nil and normalTex ~= nil then
         material:setTexture("mainTexture", normalTex)
    end
end

function onPressed()

    if render ~= nil and pressedTex ~= nil then
        material:setTexture("mainTexture", pressedTex)
    end

    if transform ~= nil and baseSize ~= nil then
        transform:setSize(Vector2.new(
            baseSize.x * (1 + (1 - pressSquish)),
            baseSize.y * pressSquish
        ))
    end

    holdTime = 0
end

function onReleased()

    if interaction ~= nil and interaction:getIsHovered() then
        material:setTexture("mainTexture", hoverTex)
    else
        material:setTexture("mainTexture", normalTex)
    end

    if transform ~= nil and baseSize ~= nil then
        transform:setSize(baseSize)
    end

end

function onHeld( )

    if transform == nil then
        log("Failed to Squish - No Trans")
        return
    end

    if baseSize == nil then
        log("Failed to Squish - No baseSize")
        return
    end

    holdTime = holdTime + 0.016

    -- Oscillating squash/stretch
    local offset = math.sin(holdTime * holdSpeed) * holdAmplitude

    transform:setSize(Vector2.new(
        baseSize.x * (1 + (1 - pressSquish)) + offset,
        baseSize.y * pressSquish - offset
    ))

end
