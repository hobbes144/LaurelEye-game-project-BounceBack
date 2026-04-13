-- Button Volume Control Script

local audioComp

local volumeType
local volumeDelta

local function clamp(v, min, max)
    if v < min then return min end
    if v > max then return max end
    return v
end

-- get current volume (0.0 - 1.0)
local function getVolume()
    if volumeType == "Master" then
        return Audio.getMasterVolume()
    elseif volumeType == "Music" then
        return Audio.getMusicVolume()
    elseif volumeType == "SFX" then
        return Audio.getSFXVolume()
    elseif volumeType == "UI" then
        return Audio.getUIVolume()
    end

    return nil
end

-- set volume (0.0 - 1.0)
local function setVolume(v)
    if volumeType == "Master" then
        Audio.setMasterVolume(v)
    elseif volumeType == "Music" then
        Audio.setMusicVolume(v)
    elseif volumeType == "SFX" then
        Audio.setSFXVolume(v)
    elseif volumeType == "UI" then
        Audio.setUIVolume(v)
    end
end

function onStart()

    audioComp = self:findAudio()

    volumeType = Serialized.volumeType
    volumeDelta = Serialized.volumeDelta

    if volumeType == nil then
        log("ERROR: volumeType not set")
    end

    if volumeDelta == nil then
        log("ERROR: volumeDelta not set")
    end

end

function onClicked()

    if volumeType == nil or volumeDelta == nil then
        return
    end

    local current = getVolume()

    if current == nil then
        log("ERROR: Invalid volumeType: " .. volumeType)
        return
    end

    local delta = volumeDelta / 100.0

    local newValue = clamp(current + delta, 0.0, 1.0)

    setVolume(newValue)

    log(volumeType .. " Volume: " .. math.floor(newValue * 100))

    if audioComp ~= nil then
        audioComp:play("demo")
    end

end
