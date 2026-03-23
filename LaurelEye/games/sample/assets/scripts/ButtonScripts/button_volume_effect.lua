--Button Menu Navigatiopn

local volumeDelta

function onStart()
    -- Pull serialized values
    volumeDelta = Serialized.volumeDelta

end

function onClicked()

    if Audio == nil then
        log("ERROR: Audio manager not found")
        return
    end

end
