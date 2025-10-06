-- Add an event listener to the main stage for Updates
---@diagnostic disable: undefined-global

-- Attach update+render listeners to a stage
-- 'stage' is the Stage object (passed from elsewhere or obtained via Core)
local M = {}

-- Global toggle to enable/disable prints from this listener file
local ENABLE_PRINTS = false

local function log(...)
    if ENABLE_PRINTS then
        print(...)
    end
end

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
            local b = Core:getDisplayObject("blueishBox")
            if b and stage:hasChild(b) then
                log("Core.run Info: Stage still has blueishBox after 25 iterations.")
            else
                log("Core.run Info: Stage no longer has blueishBox after 25 iterations.")
            end
        end
        -- Core:shutdown()
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
    log("OnInit LUA: received OnInit event")
end

function M.on_quit(evt)
    -- Called when the application is quitting; do cleanup here
    log("OnQuit LUA: received OnQuit event")
end

function M.on_event(evt)
    -- Generic event handler example
    if evt.sdl and evt.sdl.type then
        if evt.sdl.type ~= "SDL_EVENT_MOUSE_MOTION" then
            log("SDL Event: " .. evt.sdl.type .. "  name: " .. tostring(evt.name))
        end
    else
        log("LUA.on_event(): received event with no SDL payload, name: " .. tostring(evt.name))
    end
end

function M.on_click(evt)
    -- called when a mouse click event is received
    -- Helper to pretty-print DisplayObject values safely
    local function prettyHandle(h)
        if h == nil then return "nil" end
        -- DisplayObject exposes isValid() and getName() in the Lua binding
        local ok = pcall(function() return h:isValid() end)
        if ok then
            if h:isValid() then
                local name_ok, name = pcall(function() return h:getName() end)
                if name_ok and name and name ~= "" then return name end
                return "DisplayObject (valid)"
            end
            return "DisplayObject (invalid)"
        end
        -- Fallback to tostring when methods aren't available
        return tostring(h)
    end
    log("LUA: received event type: " .. evt.type .. "  name: " .. evt.name )
    if evt.sdl and evt.sdl.button then
        log("At X:" .. evt.sdl.button.x .. "  Y: " .. evt.sdl.button.y)
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
        log(prefix .. "target: " .. prettyHandle(evt.target) .. suffix)
        log("relatedTarget: " .. prettyHandle(evt.relatedTarget))
        log("currentTarget: " .. prettyHandle(evt.currentTarget))
        log("evt.sdl.button.which: " .. tostring(evt.sdl.button.which) .. "  button: " .. tostring(evt.sdl.button.button) .. "  clicks: " .. tostring(evt.sdl.button.clicks))
        -- Coerce button to a number when possible (some bindings may pass strings)
        local braw = evt.sdl.button.button
        local b = tonumber(braw) or braw
        local sdltype = evt.sdl.type or ""

        -- Helpful mapping: SDL mouse buttons are typically 1=left, 2=middle, 3=right
        local function button_name(n)
            if n == 1 then return "LEFT" end
            if n == 2 then return "MIDDLE" end
            if n == 3 then return "RIGHT" end
            return tostring(n)
        end

        -- Prefer to detect press/release via the SDL event type if available
        if sdltype == "SDL_EVENT_MOUSE_BUTTON_DOWN" then
            log("  state: PRESSED -- " .. button_name(b) .. " (" .. tostring(b) .. ")")
        elseif sdltype == "SDL_EVENT_MOUSE_BUTTON_UP" then
            log("  state: RELEASED -- " .. button_name(b) .. " (" .. tostring(b) .. ")")
            if (b == 3) then
                log("  (Right-click detected; you could trigger a test function here)")
                local parent = nil
                if evt.target and evt.target.isValid and evt.target:isValid() then
                    local ok, p = pcall(function() return evt.target:getParent() end)
                    if ok then parent = p end
                end                
                local s = getStage()
                if evt.target and evt.target.isValid and evt.target:isValid() then
                    local ok2 = pcall(function() evt.target:setOrphanRetentionPolicy('auto') end)
                end -- "auto", "grace", "manual"
                -- parent:removeChild(evt.target)  -- example: remove the clicked object from its parent
                -- parent:removeChild(evt.target:getName())  -- example: remove by name
                -- evt.target:removeFromParent()  -- remove self from parent
                if evt.target and evt.target.isValid and evt.target:isValid() then
                    s:removeDescendant(evt.target)
                end  -- remove self from stage
                -- setWindowTitle("Removed " .. prettyHandle(evt.target) .. " from stage") -- this works
                -- shutdown()
                -- quit()
            end
        end
    end
end


function M.attach(stage)
    if not stage then error("stage required") end

    -- Register named handlers on the stage using EventType table so the
    -- C++ binding recognizes the event type and the listener table form.
    stage:addEventListener({ type = EventType.OnUpdate, listener = M.on_update })
    stage:addEventListener({ type = EventType.OnRender, listener = M.on_render })
    stage:addEventListener({ type = EventType.OnPreRender, listener = M.on_prerender }) 

    -- Register listener-only events (OnInit/OnQuit/OnEvent) on the stage so global event listeners can receive them
    stage:addEventListener({ type = EventType.OnInit, listener = M.on_init })
    stage:addEventListener({ type = EventType.OnQuit, listener = M.on_quit })

    -- Listen for raw OnEvent notifications (global SDL events) 
    -- stage:addEventListener({ type = EventType.OnEvent, listener = M.on_event }) 
    stage:addEventListener({ type = EventType.SDL_Event, listener = M.on_event }) 

    -- Also listen specifically for MouseMove events so the handler sees the MouseMove event
    stage:addEventListener({ type = EventType.MouseClick, listener = M.on_click }) 

    -- Call OnInit immediately for the stage (if you want stage listeners to initialize now)
    -- Note: this is listener-only; Core/Factory also dispatch OnInit when objects are created.
    local evt = { type = "OnInit", target = stage }
    M.on_init(evt)

end

return M
