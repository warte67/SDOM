-- luacheck: globals Core
---@diagnostic disable: undefined-global
-- Callback module for window resize events
local M = {}

function M.on_window_resize(w, h)
    -- print("Window Resized--Width: " .. w .. "  Height: " .. h)
end

return M
