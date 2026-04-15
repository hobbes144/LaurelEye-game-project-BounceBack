local numEnemies = nil
local numGens = nil

function onStart()
    numEnemies = GameManager:getEnemiesKilled()
    numGens = GameManager:getGeneratorsDestroyed()
    local uiText = self:findUIText()
    local uiName = self:getName()
    if uiName == "BodyCount" then
        local strEn = string.format("%d", numEnemies)
        uiText:setText(strEn)
    elseif uiName == "Destruction" then
        local strGen = string.format("%d", numGens)
        uiText:setText(strGen)
    end

end
