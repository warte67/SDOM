-- luacheck: globals Core CLR
---@diagnostic disable: undefined-global
-- Callback module for initialization
local M = {}

function M.on_init()
    -- print(CLR.GREEN .. "Custom OnInit called!" .. CLR.RESET)

    -- Add a test: color each hex value by the bit-pattern {r:3 g:3 b:2}
    -- Print 16 values per line for readability
    print(CLR.YELLOW .. "[init.lua] Hex table function M.on_init(colorized by CLR.fg_color):" .. CLR.RESET)
    for i = 0, 255 do
        if i > 127 then
            io.write(CLR.BLACK) -- switch to black text for light bg
        end
        -- print colored token using the C++ helper that maps 8-bit -> 24-bit
        io.write(CLR.bg_color(i) .. " " .. CLR.hex(i, 2) .. " ".. CLR.RESET)

        if i < 255 then
            if ((i + 1) % 16) == 0 then
                io.write("\n")
            else
                io.write(" ")
            end
        end
    end
    io.write("\n")
    
    -- attach per-stage listeners (delegate to the shared listener_callbacks module)
    local listeners = require("callbacks.listener_callbacks")
    local stage = Core and Core:getStage() or Core and Core:getDisplayObject("mainStage")
    if stage then
        listeners.attach(stage)
    else
        print("Warning: main stage not found; stage listeners not attached")
    end

    return true
end

return M
