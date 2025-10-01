-- Add an event listener to the main stage for Updates
-- Attach update+render listeners to a stage
-- 'stage' is the Stage object (passed from elsewhere or obtained via Core)
local M = {}

-- rolling FPS average state and smoothing factor (match render.lua behavior)
local ema = nil
local EMA_ALPHA = 2.0 / (100.0 + 1.0)

-- Named listener handlers so they can be referenced from other modules or tested directly
function M.on_update(evt)
    local dt = (evt and evt.dt) or (Core and Core:getElapsedTime() ) or 0
    -- put per-frame stage logic here
    -- e.g. animate children, update physics, etc.
    -- print("OnUpdate LUA: " .. tostring(dt))
end

function M.on_render(evt)
    -- draw into the stage (use display objects or CLR.draw_debug_text for debug overlays)
    -- print("OnRender LUA")

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

    -- print("OnRender LUA: " .. evt:getName())

    -- Render FPS into SDL window using CLR.draw_debug_text. Draw at top-left inside the window.
    -- Parameters: text, x, y, ptsize, r,g,b,a
    CLR.draw_debug_text(string.format("FPS: %d", display), 8, 8, 14, 255, 255, 255, 255)
end

function M.on_init(evt)
    -- Called when an object (or stage) receives OnInit via event listeners
    print("OnInit LUA: received OnInit event")
    -- evt.relatedTarget may contain the stage or parent info
end

function M.on_quit(evt)
    -- Called when the application is quitting; do cleanup here
    -- print("OnQuit LUA: received OnQuit event")
end

function M.on_event(evt)
    -- Generic event handler example
    -- print(string.format("OnEvent LUA: type=%s target=%s", tostring(evt and evt.type or ""), tostring(evt and evt.target and evt.target:getName() or "<nil>")))
end


function M.attach(stage)
    if not stage then error("stage required") end

    -- Register named handlers on the stage using EventType table so the
    -- C++ binding recognizes the event type and the listener table form.
    stage:addEventListener({ type = EventType.OnUpdate, listener = M.on_update })  -- VERIFIED
    stage:addEventListener({ type = EventType.OnRender, listener = M.on_render })  -- VERIFIED
    -- stage:addEventListener({ type = EventType.OnPreRender, listener = M.on_render })  -- OnPreRender  VERIFIED

    -- Register listener-only events (OnInit/OnQuit/OnEvent) on the stage so global event listeners can receive them
    stage:addEventListener({ type = EventType.OnInit, listener = M.on_init })   -- UNLIKELY
    stage:addEventListener({ type = EventType.OnQuit, listener = M.on_quit })   -- VERIFIED
    stage:addEventListener({ type = EventType.OnEvent, listener = M.on_event })  -- VERIFIED

    -- Optionally, call OnInit immediately for the stage (if you want stage listeners to initialize now)
    -- Note: this is listener-only; Core/Factory also dispatch OnInit when objects are created.
    -- local evt = { type = "OnInit", target = stage }
    -- M.on_init(evt)
end

return M
