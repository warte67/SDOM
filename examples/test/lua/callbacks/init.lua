-- luacheck: globals Core CLR
---@diagnostic disable: undefined-global
-- Callback module for initialization
local M = {}

function M.on_init()
    print(CLR.GREEN .. "Custom OnInit called!" .. CLR.RESET)

    -- Add a test: color each hex value by the bit-pattern {r:3 g:3 b:2}
    -- Print 16 values per line for readability
    print(CLR.YELLOW .. "Hex table (colorized by CLR.fg_color):" .. CLR.RESET)
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
    
    -- attach per-stage listeners (update + render)
    local stage = Core and Core:getStageHandle() or Core and Core:getDisplayObject("mainStage")
    if stage then
        -- Update listener: receives evt.dt if posted, otherwise use Core:getElapsedTime()
        stage:addEventListener("Update", function(evt)
            local dt = (evt and evt.dt) or (Core and Core:getElapsedTime()) or 0
            -- per-frame logic here
            -- e.g. animate or update state using dt
        end)

        -- Render listener: draws as part of the stage texture (will be cleared by stage)
        stage:addEventListener("Render", function(evt)
            -- Draw into the stage (use display objects or CLR.draw_debug_text for debug)
            -- Example debug text at 8,8 inside the stage:
            CLR.draw_debug_text(string.format("Stage Render at t=%.2f", Core and Core:getElapsedTime() or 0), 8, 8, 14, 255,255,255,255)
        end)
    else
        print("Warning: main stage not found; stage listeners not attached")
    end

    return true
end

return M
