-- luacheck: globals Core CLR
---@diagnostic disable: undefined-global
-- Callback module for event handling
local M = {}

function M.on_event(e)
    -- Simple debug print; keep lightweight in production
    -- print(CLR.GREEN .. "Event received!" .. CLR.RESET)
end

return M
