-- Add an event listener to the main stage for Updates
-- Attach update+render listeners to a stage
-- 'stage' is the Stage object (passed from elsewhere or obtained via Core)
local M = {}
function M.attach(stage)
    if not stage then error("stage required") end

    -- Update listener: receives an event table (use evt.dt or Core:getElapsedTime())
    stage:addEventListener("Update", function(evt)
        local dt = (evt and evt.dt) or (Core and Core:getElapsedTime() ) or 0
        -- put per-frame stage logic here
        -- e.g. animate children, update physics, etc.
    end)

    -- Render listener: called while rendering the stage (draw on stage texture)
    stage:addEventListener("Render", function(evt)
        -- draw into the stage (use display objects or CLR.draw_debug_text if you want debug overlays
        -- attached to the stage they will be part of the texture)
        print("Stage Render callback")
    end)
end