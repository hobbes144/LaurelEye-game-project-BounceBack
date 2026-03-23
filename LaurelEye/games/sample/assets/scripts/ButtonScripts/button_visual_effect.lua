--Button Menu Visuals

local renderUI
local transformUI
local interactionUI

local normalColor
local hoverColor
local pressedColor

--Compression

local baseSize
local holdTime = 0

-- Tuning values
local pressSquish = 0.95
local holdAmplitude = 1.5
local holdSpeed = 20.0

function onStart()

    -- Get components using YOUR bindings
    render = self:findUIRender()
    transform = self:findUITransform()
    interaction = self:findUIInteraction()

    -- Pull serialized values
    hoverColor = Serialized.hoverColor
    pressedColor = Serialized.pressedColor

    if render == nil then
        log("ERROR: No UIRenderComponent found")
    end

    if render ~= nil then
        normalColor = render:getColor()
    end

    if transform ~= nil then
        baseSize = transform:getSize()
    end

end

function onHoverEnter()
    log("Hover Visual Enter")

    if render ~= nil and hoverColor ~= nil then
        log("Changing Color")
        render:setColor(hoverColor)
    end
end

function onHoverExit()
    if render ~= nil and normalColor ~= nil then
        render:setColor(normalColor)
    end
end

function onPressed()

    if render ~= nil and pressedColor ~= nil then
        render:setColor(pressedColor)
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
        render:setColor(hoverColor)
    else
        render:setColor(normalColor)
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
