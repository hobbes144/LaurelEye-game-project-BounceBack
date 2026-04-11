--Button Menu Navigatiopn

local renderUI
local transformUI
local interactionUI

local targetScene
local timeToTransition

local elapsedTime = 0
local hasTriggered = false

function onStart()

    -- Get components using YOUR bindings
    render = self:findUIRender()
    transform = self:findUITransform()
    interaction = self:findUIInteraction()

    -- Pull serialized values
    targetScene = Serialized.targetScene
    timeToTransition = Serialized.timeToTransition

    if render ~= nil and normalColor ~= nil then
        normalColor = render:getColor()
    end

    if transform ~= nil then
        baseScale = transform:getSize()
    end

end

function onUpdate(dt)

    -- Only run if timeToTransition is defined
    if timeToTransition ~= nil and not hasTriggered then
        elapsedTime = elapsedTime + dt

        if elapsedTime >= timeToTransition then
            triggerSceneChange()
        end
    end

end

function onHoverEnter()

    log("Navigation Hover Enter")

end

function onClicked()
    if timeToTransition ~= nil and not hasTriggered then
        log("Timer skipped via click")
        triggerSceneChange()
        return
    end

    if targetScene == nil then
        log("ERROR: targetScene is nil")
        return
    end

    triggerSceneChange()

end

function triggerSceneChange()

    if hasTriggered then return end
    hasTriggered = true

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
