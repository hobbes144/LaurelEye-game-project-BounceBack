tagetScene = 0;

function onStart()
    
end
--End OnStart()

function onHoverEnter()
    log("Button Hover Enter")
end

function onHoverExit()
    log("Button Hover Exit")
end

function onPressed( )
	log("Button Pressed")
end

function onClicked()
    log("Button Clicked")

    log("Loading Scene: Level1")
    Window.setCursorMode(0, Window.CursorMode.Disabled)
    SceneManager:changeScene("Level1")

end

function onReleased()
    log("Button Released")
end

function onHeld( )
	log("Button Held")
end

function onDragStart( )
	log("Button Drag Start")
end

function onDragging( )
	log("Button Dragging")
end

function onDragEnd( )
	log("Button Drag End")
end
