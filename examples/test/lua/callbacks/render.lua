-- luacheck: globals Core CLR
---@diagnostic disable: undefined-global
-- Callback module for custom rendering
local M = {}

-- rolling FPS average over the last N frames
-- Exponential moving average (EMA) parameters
local ema = nil
local EMA_ALPHA = 2.0 / (100.0 + 1.0) -- smooth roughly equivalent to 100-frame window

function M.on_render()
    -- Custom rendering steps can go here
    local instant = 0
    local dt = Core:getElapsedTime()
    if dt and dt > 0 then instant = 1.0 / dt end
    if ema == nil then
        ema = instant
    else
        ema = EMA_ALPHA * instant + (1.0 - EMA_ALPHA) * ema
    end
    local display = math.floor(ema + 0.5)

    -- Render FPS into SDL window using CLR.draw_debug_text. Draw at top-left inside the window.
    -- Parameters: text, x, y, ptsize, r,g,b,a
    CLR.draw_debug_text(string.format("FPS: %d", display), 8, 8, 14, 255, 255, 255, 255)
end

return M
