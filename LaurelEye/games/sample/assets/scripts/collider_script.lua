generator = nil

function onStart()
end

function onMessage(msg)
    if msg.topic == "Tis I!" then
        generator = msg.from
        print("Generator: ", generator)
    end

end


function onTriggerEnter(data)
    local tagsA = data.entityA:getTags()
    for _, tag in pairs(tagsA) do
        if tag == "ball" then
            local message = Message.new()
            message.to = generator
            message.topic = "Change Texture!"
            Script.send(message)
        end
    end

    local tagsB = data.entityB:getTags()
    for _, tag in pairs(tagsB) do
        if tag == "ball" then
            local message = Message.new()
            message.to = generator
            message.topic = "Change Texture!"
            Script.send(message)
        end
    end

end
