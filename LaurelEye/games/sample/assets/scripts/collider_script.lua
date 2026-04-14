generator = nil

function onStart()
end

function onMessage(msg)
    if msg.topic == "Tis I!" then
        generator = msg.from
        print("Generator: ", generator)
    end

    if msg.topic == "Get Destroyed!" then
        local message = Message.new()
        message.to = generator
        message.topic = "Change Texture!"
        Script.send(message)
    end
end
