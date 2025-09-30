-- luacheck: globals Core CLR
---@diagnostic disable: undefined-global
-- Callback module for quit/shutdown
local M = {}

function M.on_quit()
    -- print(CLR.GREEN .. "Custom OnQuit called!" .. CLR.RESET)
end

return M
