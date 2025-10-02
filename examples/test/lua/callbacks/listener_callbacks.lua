-- Add an event listener to the main stage for Updates
---@diagnostic disable: undefined-global

-- Attach update+render listeners to a stage
-- 'stage' is the Stage object (passed from elsewhere or obtained via Core)
local M = {}

-- rolling FPS average state and smoothing factor (match render.lua behavior)
local ema = nil
local EMA_ALPHA = 2.0 / (100.0 + 1.0)

local s_iterations = 0

-- Named listener handlers so they can be referenced from other modules or tested directly
function M.on_update(evt)
    local dt = (evt and evt.dt) or (Core and Core:getElapsedTime() ) or 0
    -- put per-frame stage logic here

    
    -- call this each frame / update
    s_iterations = s_iterations + 1

    if s_iterations == 25 then
        local stage = Core:getStageHandle()
        if stage then
            local b = Core:getDisplayObjectHandle("blueishBox")
            if b and stage:hasChild(b) then
                print("Core.run Info: Stage still has blueishBox after 25 iterations.")
            else
                print("Core.run Info: Stage no longer has blueishBox after 25 iterations.")
            end
        end
        Core:shutdown()
    end
end

function M.on_prerender(evt)
    -- Called BEFORE children are rendered; but after the stage is rendered
    CLR.draw_debug_text("Stage", 550, 8, 14, 255, 64, 64, 255)
end

function M.on_render(evt)
    -- Called AFTER children are rendered; used as a final overlay

    -- Calculate and display smoothed FPS value
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

function M.on_init(evt)
    -- Called when an object (or stage) receives OnInit via event listeners
    -- evt.relatedTarget may contain the stage or parent info
    print("OnInit LUA: received OnInit event")
end

function M.on_quit(evt)
    -- Called when the application is quitting; do cleanup here
    print("OnQuit LUA: received OnQuit event")
end

function M.on_event(evt)
    -- Generic event handler example
    if evt.sdl and evt.sdl.type then
        if evt.sdl.type ~= "SDL_EVENT_MOUSE_MOTION" then
            print("SDL Event: " .. evt.sdl.type .. "  name: " .. tostring(evt.name))
        end
    else
        print("LUA.on_event(): received event with no SDL payload, name: " .. tostring(evt.name))
    end
end

function M.on_click(evt)
    -- called when a mouse click event is received
    -- Helper to pretty-print DomHandle values safely
    local function prettyHandle(h)
        if h == nil then return "nil" end
        -- DomHandle exposes isValid() and getName() in the Lua binding
        local ok = pcall(function() return h:isValid() end)
        if ok then
            if h:isValid() then
                local name_ok, name = pcall(function() return h:getName() end)
                if name_ok and name and name ~= "" then return name end
                return "DomHandle(valid)"
            end
            return "DomHandle(invalid)"
        end
        -- Fallback to tostring when methods aren't available
        return tostring(h)
    end
    print("LUA: received event type: " .. evt.type .. "  name: " .. evt.name )
    if evt.sdl and evt.sdl.button then
        print("At X:" .. evt.sdl.button.x .. "  Y: " .. evt.sdl.button.y)
        -- Colorize the "target:" label using the target's color if available
        local function colorPrefixForHandle(h)
            if not h then return "" end
            local ok, c = pcall(function() return h:getColor() end)
            if ok and c then
                -- SDL_Color exposes r,g,b (0-255)
                local ok2, seq = pcall(function() return CLR.fg_rgb(c.r, c.g, c.b) end)
                if ok2 and seq then return seq end
            end
            return ""
        end

        local prefix = colorPrefixForHandle(evt.target)
        local suffix = prefix ~= "" and CLR.RESET or ""
        print(prefix .. "target: " .. prettyHandle(evt.target) .. suffix)
        print("relatedTarget: " .. prettyHandle(evt.relatedTarget))
        print("currentTarget: " .. prettyHandle(evt.currentTarget))
        print("evt.sdl.button.which: " .. tostring(evt.sdl.button.which) .. "  button: " .. tostring(evt.sdl.button.button) .. "  clicks: " .. tostring(evt.sdl.button.clicks))
    end
end


function M.attach(stage)
    if not stage then error("stage required") end

    -- Register named handlers on the stage using EventType table so the
    -- C++ binding recognizes the event type and the listener table form.
    stage:addEventListener({ type = EventType.OnUpdate, listener = M.on_update })  -- VERIFIED
    stage:addEventListener({ type = EventType.OnRender, listener = M.on_render })  -- VERIFIED
    stage:addEventListener({ type = EventType.OnPreRender, listener = M.on_prerender })  -- VERIFIED

    -- Register listener-only events (OnInit/OnQuit/OnEvent) on the stage so global event listeners can receive them
    stage:addEventListener({ type = EventType.OnInit, listener = M.on_init })   -- VERIFIED
    stage:addEventListener({ type = EventType.OnQuit, listener = M.on_quit })   -- VERIFIED

    -- Listen for raw OnEvent notifications (global SDL events) 
    -- stage:addEventListener({ type = EventType.OnEvent, listener = M.on_event })  -- in-progress
    stage:addEventListener({ type = EventType.SDL_Event, listener = M.on_event })  -- in-progress

    -- Also listen specifically for MouseMove events so the handler sees the MouseMove event
    stage:addEventListener({ type = EventType.MouseClick, listener = M.on_click }) -- VERIFIED

    -- Call OnInit immediately for the stage (if you want stage listeners to initialize now)
    -- Note: this is listener-only; Core/Factory also dispatch OnInit when objects are created.
    local evt = { type = "OnInit", target = stage }
    M.on_init(evt)

end

return M
