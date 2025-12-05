local renderable = nil

local delay = 3.0        -- seconds before deactivation
local timer = 0.0
local doFadeOut = false  -- change to true when transparency is implemented

function onStart()

    -- Optional: get material (if fade enabled)
    --if doFadeOut then
    --    local rend = self:findRender3D()
    --    if rend ~= nil and rend:hasMaterial() then
    --        renderable = rend:getMaterial()
    --    end
    --end
end

function onUpdate(dt)
    timer = timer + dt

    -- if doFadeOut and renderable ~= nil then
    --     local t = math.min(timer / delay, 1.0)
    --     local alpha = 1.0 - t       -- fade from 1 → 0
    --     renderable:setProperty("alpha", alpha)
    -- end

    -- When timer ends, deactivate entity
    if timer >= delay or Input:isKeyPressed(Key.Tab) then
        self:setActive(false)
    end
end

function onShutdown()
    -- Nothing needed
end
