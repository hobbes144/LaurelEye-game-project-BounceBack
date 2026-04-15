-- Cutscene Fade Script

local render
local interaction

local temporary
local delay
local fadeDuration

local elapsed = 0
local fading = false
local done = false

local startAlpha = 1.0

function onStart()

    render = self:findUIRender()
    interaction = self:findUIInteraction()

    temporary = Serialized.temporary or false
    delay = Serialized.delay or 1.0
    fadeDuration = Serialized.fadeDuration or 1.0

    if render ~= nil then
        startAlpha = render:getTransparency()
    end

end

function onUpdate(dt)

    if done then return end

    elapsed = elapsed + dt

    -- Start fade after delay
    if not fading and elapsed >= delay then
        fading = true
        elapsed = 0
    end

    -- Handle fading
    if fading then
        local t = elapsed / fadeDuration
        if t > 1.0 then t = 1.0 end

        local alpha = startAlpha * (1.0 - t)

        audio = self:findAudio()
        if audio ~= nil then
            audio:play("gasp")
        end

        if render ~= nil then
            render:setTransparency(alpha)
        end

        if t >= 1.0 then
            finish()
        end
    end

end

function onClicked()
    skip()
end

function skip()
    if done then return end

    log("Cutscene skipped")

    -- Instantly hide
    if render ~= nil then
        render:setTransparency(0.0)
    end

    finish()
end

function finish()

    if done then return end
    done = true

    if temporary then

        SceneManager:destroy(self)

    end

end
