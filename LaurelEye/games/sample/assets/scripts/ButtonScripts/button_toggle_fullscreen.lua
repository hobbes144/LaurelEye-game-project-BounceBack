--Button Menu Navigatiopn

local fullscreen = false

function onClicked()

    fullscreen = not fullscreen
    Window.setFullscreen(0, fullscreen)

end
