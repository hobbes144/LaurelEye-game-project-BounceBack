healthUIElements = {}

function onStart()
    -- Assume there are 3 health UI elements; adjust count as needed
    for i = 1, 3 do
        healthUIElements[i] = UIElementManager:findUIComponent("HealthUI" .. i)
    end
end

function onUpdate(dt)
    local health = self:getPlayerHealth()
    for i, uiElement in ipairs(healthUIElements) do
        if uiElement then
            if i <= health then
                uiElement:setActive(true)
            else
                uiElement:setActive(false)
            end
        end
end
