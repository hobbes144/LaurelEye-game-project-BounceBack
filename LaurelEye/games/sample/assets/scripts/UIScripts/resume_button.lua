--Button Menu Navigatiopn

local hudController = nil

function onStart()

    local scene = SceneManager:getCurrentScene()
    hudController = scene:findEntityByName(Serialized.HUDControllerName or "HUDController")

    if hudController ~= nil then
        
    else
        log("ERROR: No HUDController Found")
    end

end


function onClicked()

    log("---------------=MANUAL RESUME=-----------------")
    local message = Message.new()
    message.to = hudController
    message.topic = "Resume Button Clicked"
    Script.send(message)

end
