wPressed = false
aPressed = false
sPressed = false
dPressed = false

function onUpdate(dt)
    wPressed = Input:isKeyPressed(Key.W)
    aPressed = Input:isKeyPressed(Key.A)
    sPressed = Input:isKeyPressed(Key.S)
    dPressed = Input:isKeyPressed(Key.D)

    if Input:isButtonPressed(GamepadButton.A) then
        log("Pressed the A button on your controller. Good job you!")
    end
    if Input:isKeyHeld(Key.Space) then
        local x, y = Input:getMousePosition()
        log("Currently at position: " .. x .. ", " .. y)
    end
end
