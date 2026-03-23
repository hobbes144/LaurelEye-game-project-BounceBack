--Button Menu Navigatiopn

local renderUI
local transformUI
local interactionUI

local targetScene

function onStart()

    -- Get components using YOUR bindings
    render = self:findUIRender()
    transform = self:findUITransform()
    interaction = self:findUIInteraction()

    -- Pull serialized values
    targetScene = Serialized.targetScene

    if render ~= nil and normalColor ~= nil then
        normalColor = render:getColor()
    end

    if transform ~= nil then
        baseScale = transform:getSize()
    end

end

function onHoverEnter()

    log("Navigation Hover Enter")

end

function onClicked()
    if targetScene == nil then
        log("ERROR: targetScene is nil")
        return
    end

    log("Loading Scene: " .. targetScene)

    if targetScene == "Quit" then
        Engine:stop()
        return
    end

    if targetScene == "Reload" or targetScene == "" then
        SceneManager:reloadCurrentScene()
        return
    end

    SceneManager:changeScene(targetScene)

end
