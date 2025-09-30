-- luacheck: globals Core CLR
---@diagnostic disable: undefined-global
-- Callback module for per-frame updates
local M = {}

function M.on_update(dt)
    -- Uncomment the print for debugging; it can be chatty during runtime.
    -- print(CLR.BLUE .. "Custom Update called! Elapsed time: " .. dt .. CLR.RESET)
end

return M
