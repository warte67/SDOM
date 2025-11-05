-- EventType_UnitTests.lua

-- Do not use print statements in unit test scripts, as they may interfere with test output.
-- print("EventType_UnitTests script entered successfully.")

-- Push Errors (Example errors; replace with actual test results) instead of print statements
-- utils.push_error("widget X did not initialize")
-- utils.push_error("asset Y missing")

-- require the helper
local utils = require("src.UnitTests")

-- Small helpers
local function push(msg) utils.push_error(msg) end
local function assert_true(cond, msg) if not cond then push(msg) end end
local function assert_false(cond, msg) if cond then push(msg) end end
---@param h DisplayHandle|nil
---@return boolean h_is_valid
local function is_valid(h) return (h ~= nil) and (h.isValid == nil or h:isValid()) end

-- Get common objects configured in examples/test/lua/config.lua
local function get_stage()
    local s = Core:getDisplayObject("mainStage")
    if not is_valid(s) then push("mainStage not found") end
    return s
end
local function get_blueish()
    local b = Core:getDisplayObject("blueishBox")
    if not is_valid(b) then push("blueishBox not found") end
    return b
end
local function get_blueish_label()
    local l = Core:getDisplayObject("blueishBoxLabel")
    if not is_valid(l) then push("blueishBoxLabel not found") end
    return l
end

-- Re-entrant test state and API (used each frame by callbacks.unittest)
local M = {}
local S = {
    t1 = { init=false, phase="init", frames=0, stage=nil, move_hit=false, listener=nil, start_frame=0 },
    t2 = { init=false, phase="init", frames=0, stage=nil, kd=false, ku=false, start_frame=0 },
    t3 = { init=false, phase="init", frames=0, stage=nil, down=false, up=false, move=false, wheel=false, start_frame=0 },
    t4 = { init=false, phase="init", frames=0, stage=nil, click=false, dclick=false, start_frame=0 },
    t5 = { init=false, phase="init", frames=0, stage=nil, enter=false, leave=false, start_frame=0 },
    t6 = { init=false, phase="init", frames=0, stage=nil, hits=nil, start_frame=0, orig=nil, was_fullscreen=false, move_phase="none", move_wait=0 },
    t7 = { init=false, phase="init", frames=0, stage=nil, slider=nil, vhit=false, shit=false, start_frame=0 },
    t8 = { init=false, phase="init", frames=0, stage=nil, hits=nil, parent=nil, child=nil, start_frame=0 },
}

-- EventType_test0: Placeholder template (immediate success)
function M.EventType_test0()
    return true -- completed
end

-- EventType_test6: Window environment roundtrip (Enter/LeaveFullscreen, Show/Hide, Resize, Move)
function M.EventType_test6()
    local st = S.t6
    if not st.init then
        st.init = true
        st.phase = "setup"
        st.frames = 0
        st.move_phase = "none"
    end

    if st.phase == "setup" then
        st.stage = get_stage(); if not is_valid(st.stage) then push("EventType_test6: mainStage missing"); return true end
        st.hits = { enter=false, leave=false, show=false, hide=false, resize=false, move=false }
        st.le = function(_) st.hits.enter = true end
        st.lv = function(_) st.hits.leave = true end
        st.ls = function(_) st.hits.show  = true end
        st.lh = function(_) st.hits.hide  = true end
        st.lr = function(_) st.hits.resize= true end
        st.lm = function(_) st.hits.move  = true end
        st.stage:addEventListener({ type = EventType.EnterFullscreen, listener = st.le })
        st.stage:addEventListener({ type = EventType.LeaveFullscreen, listener = st.lv })
        st.stage:addEventListener({ type = EventType.Show,            listener = st.ls })
        st.stage:addEventListener({ type = EventType.Hide,            listener = st.lh })
        st.stage:addEventListener({ type = EventType.Resize,          listener = st.lr })
        st.stage:addEventListener({ type = EventType.Move,            listener = st.lm })
        if Core and Core.getFrameCount then local fc = Core:getFrameCount(); if type(fc)=="number" then st.start_frame = fc end end
        -- capture original state
        st.was_fullscreen = Core.isFullscreen and Core:isFullscreen() or false
        local pos = Core.getWindowPosition and Core:getWindowPosition() or {x=0,y=0}
        st.orig = { x = pos.x or 0, y = pos.y or 0, w = Core:getWindowWidth(), h = Core:getWindowHeight() }
        -- ensure windowed baseline
        if Core.setWindowed then Core:setWindowed(true) end
        st.phase = "enter_full"
        return false
    end

    if st.phase == "enter_full" then
        -- request fullscreen
        if Core.setFullscreen then Core:setFullscreen(true) end
        st.phase = "check_enter"
        return false
    end

    if st.phase == "check_enter" then
        if not st.hits.enter then push("EventType_test6: Missing EnterFullscreen after setFullscreen(true)") end
        if not (st.hits.show or st.hits.resize) then push("EventType_test6: Expected Show or Resize after entering fullscreen") end
        st.hits = { enter=false, leave=false, show=false, hide=false, resize=false, move=false }
        if Core.setWindowed then Core:setWindowed(true) end
        st.phase = "check_leave"
        return false
    end

    if st.phase == "check_leave" then
        if not st.hits.leave then push("EventType_test6: Missing LeaveFullscreen after setWindowed(true)") end
        if not (st.hits.hide or st.hits.resize) then push("EventType_test6: Expected Hide or Resize after leaving fullscreen") end
        st.hits = { enter=false, leave=false, show=false, hide=false, resize=false, move=false }
        st.move_phase = "settle"
        st.move_wait = 0
        st.phase = "move"
        return false
    end

    if st.phase == "move" then
        if st.move_phase == "settle" then
            st.move_wait = st.move_wait + 1
            if st.move_wait >= 12 then st.move_phase = "start" end
            return false
        elseif st.move_phase == "start" then
            local pos = Core.getWindowPosition and Core:getWindowPosition() or {x=0,y=0}
            Core.setWindowPosition(Core, { x = (pos.x or 0) + 24, y = (pos.y or 0) + 24 })
            st.move_phase = "wait"
            st.move_wait = 0
            return false
        elseif st.move_phase == "wait" then
            st.move_wait = st.move_wait + 1
            if st.hits.move then
                st.move_phase = "done"
                st.phase = "cleanup"
                return false
            end
            if st.move_wait > 250 then
                push("EventType_test6: Move event not detected (compositor-controlled)")
                st.move_phase = "done"
                st.phase = "cleanup"
                return false
            end
            return false
        end
    end

    if st.phase == "cleanup" then
        if is_valid(st.stage) then
            st.stage:removeEventListener({ type = EventType.EnterFullscreen, listener = st.le })
            st.stage:removeEventListener({ type = EventType.LeaveFullscreen, listener = st.lv })
            st.stage:removeEventListener({ type = EventType.Show,            listener = st.ls })
            st.stage:removeEventListener({ type = EventType.Hide,            listener = st.lh })
            st.stage:removeEventListener({ type = EventType.Resize,          listener = st.lr })
            st.stage:removeEventListener({ type = EventType.Move,            listener = st.lm })
        end
        if Core and st.orig then
            if Core.setWindowPosition then Core:setWindowPosition({ x = st.orig.x, y = st.orig.y }) end
            if Core.setWindowWidth then Core:setWindowWidth(st.orig.w) end
            if Core.setWindowHeight then Core:setWindowHeight(st.orig.h) end
            if st.was_fullscreen and Core.setFullscreen then Core:setFullscreen(true) end
        end
        st.phase = "done"
        return true
    end

    return true
end

-- EventType_test7: UI/State — ValueChanged (Slider) and StateChanged (CheckButton)
function M.EventType_test7()
    local st = S.t7
    if not st.init then
        st.init = true
        st.phase = "setup"
        st.frames = 0
    end

    if st.phase == "setup" then
        st.stage = get_stage(); if not is_valid(st.stage) then push("EventType_test7: mainStage missing"); return true end
        st.vhit, st.shit = false, false
        -- Create a temporary Slider and attach ValueChanged
        st.slider = Core:createDisplayObject("Slider", { name = "lua_slider_evt", type = "Slider", x = 40, y = 420, width = 120, height = 16 })
        if not is_valid(st.slider) then push("EventType_test7: failed to create Slider"); return true end
        st.stage:addChild(st.slider)
        st.lv = function(_) st.vhit = true end
        st.slider:addEventListener({ type = EventType.ValueChanged, listener = st.lv })
        -- Attach StateChanged on the existing CheckButton in config
        st.chk = Core:getDisplayObject("mainFrame_CheckButton_1")
        if not is_valid(st.chk) then push("EventType_test7: mainFrame_CheckButton_1 missing"); return true end
        st.ls = function(_) st.shit = true end
        st.chk:addEventListener({ type = EventType.StateChanged, listener = st.ls })
        if Core and Core.getFrameCount then local fc = Core:getFrameCount(); if type(fc)=="number" then st.start_frame = fc end end
        -- Trigger ValueChanged via property assignment (IRangeControl)
        st.slider.value = 50
        -- Trigger StateChanged by clicking the CheckButton center
        if Core and Core.pushMouseEvent then
            local left = st.chk:getLeft(); local top = st.chk:getTop();
            local w = st.chk:getWidth(); local h = st.chk:getHeight();
            local cx = left + math.floor(w/2); local cy = top + math.floor(h/2);
            Core:pushMouseEvent({ x = cx, y = cy, type = "move" })
            Core:pushMouseEvent({ x = cx, y = cy, type = "down", button = 1 })
            Core:pushMouseEvent({ x = cx, y = cy, type = "up",   button = 1 })
        end
        st.phase = "wait"
        return false
    end

    if st.phase == "wait" then
        st.frames = st.frames + 1
        local delta = st.frames; if Core and Core.getFrameCount and type(st.start_frame)=="number" then local now = Core:getFrameCount(); if type(now)=="number" then delta = now - st.start_frame end end
        if st.vhit and st.shit then
            -- Cleanup
            if is_valid(st.slider) then st.slider:removeEventListener({ type = EventType.ValueChanged, listener = st.lv }) end
            if is_valid(st.chk) then st.chk:removeEventListener({ type = EventType.StateChanged, listener = st.ls }) end
            -- Remove the temporary slider
            Core:destroyDisplayObject("lua_slider_evt")
            st.phase = "done"
            return true
        end
        if delta > 120 then
            if not st.vhit then push("EventType_test7: ValueChanged not observed within timeout") end
            if not st.shit then push("EventType_test7: StateChanged not observed within timeout") end
            if is_valid(st.slider) then st.slider:removeEventListener({ type = EventType.ValueChanged, listener = st.lv }) end
            if is_valid(st.chk) then st.chk:removeEventListener({ type = EventType.StateChanged, listener = st.ls }) end
            Core:destroyDisplayObject("lua_slider_evt")
            st.phase = "done"
            return true
        end
        return false
    end

    return true
end

-- EventType_test8: Application Lifecycle — Added/Removed (+ AddedToStage/RemovedFromStage)
function M.EventType_test8()
    local st = S.t8
    if not st.init then
        st.init = true
        st.phase = "setup"
        st.frames = 0
    end

    if st.phase == "setup" then
        st.stage = get_stage(); if not is_valid(st.stage) then push("EventType_test8: mainStage missing"); return true end
        st.parent = Core:createDisplayObject("Box", { name = "lua_parent", type = "Box", x = 20, y = 460, width = 40, height = 40, color = {255,0,255,255} })
        st.child  = Core:createDisplayObject("Box", { name = "lua_child",  type = "Box", x = 4,  y = 4,   width = 16, height = 16, color = {255,0,0,255} })
        if not (is_valid(st.parent) and is_valid(st.child)) then push("EventType_test8: failed to create parent/child Box"); return true end
        st.hits = { added=false, removed=false, addedToStage=false, removedFromStage=false }
        st.ladd  = function(_) st.hits.added = true end
        st.lrem  = function(_) st.hits.removed = true end
        st.lats  = function(_) st.hits.addedToStage = true end
        st.lrfs  = function(_) st.hits.removedFromStage = true end
        st.child:addEventListener({ type = EventType.Added,            listener = st.ladd })
        st.child:addEventListener({ type = EventType.Removed,          listener = st.lrem })
        st.child:addEventListener({ type = EventType.AddedToStage,     listener = st.lats })
        st.child:addEventListener({ type = EventType.RemovedFromStage, listener = st.lrfs })
        st.parent:addChild(st.child)
        st.stage:addChild(st.parent)
        st.phase = "verify_added"
        return false
    end

    if st.phase == "verify_added" then
        if not st.hits.added then push("EventType_test8: Added did not fire for child") end
        if not st.hits.addedToStage then push("EventType_test8: AddedToStage did not fire for child") end
        st.hits.added = false; st.hits.addedToStage = false
        st.parent:removeChild(st.child)
        st.phase = "verify_removed"
        return false
    end

    if st.phase == "verify_removed" then
        if not st.hits.removed then push("EventType_test8: Removed did not fire for child") end
        if not st.hits.removedFromStage then push("EventType_test8: RemovedFromStage did not fire for child") end
        st.stage:removeChild(st.parent)
        -- Cleanup
        Core:destroyDisplayObject("lua_child")
        Core:destroyDisplayObject("lua_parent")
        st.phase = "done"
        return true
    end

    return true
end

-- EventType_test4: MouseClick and MouseDoubleClick (uses optional 'clicks' in push)
function M.EventType_test4()
    local st = S.t4
    if not st.init then
        st.init = true
        st.phase = "setup"
        st.frames = 0
    end

    if st.phase == "setup" then
        st.stage = get_stage(); if not is_valid(st.stage) then push("EventType_test4: mainStage missing"); return true end
        st.click, st.dclick = false, false
        st.lc = function(_) st.click = true end
        st.ld = function(_) st.dclick = true end
        st.stage:addEventListener({ type = EventType.MouseClick,       listener = st.lc })
        st.stage:addEventListener({ type = EventType.MouseDoubleClick, listener = st.ld })
        -- baseline
        if Core and Core.getFrameCount then local fc = Core:getFrameCount(); if type(fc)=="number" then st.start_frame = fc end end
        -- Move cursor over blueishBox then click and double-click
        if Core and Core.pushMouseEvent then
            Core:pushMouseEvent({ x = 200, y = 100, type = "move" })
            Core:pushMouseEvent({ x = 200, y = 100, type = "down", button = 1, clicks = 1 })
            Core:pushMouseEvent({ x = 200, y = 100, type = "up",   button = 1, clicks = 1 })
            -- double click: set clicks=2 on the up event
            Core:pushMouseEvent({ x = 200, y = 100, type = "down", button = 1, clicks = 2 })
            Core:pushMouseEvent({ x = 200, y = 100, type = "up",   button = 1, clicks = 2 })
        end
        st.phase = "wait"
        return false
    end

    if st.phase == "wait" then
        st.frames = st.frames + 1
        local delta = st.frames
        if Core and Core.getFrameCount and type(st.start_frame) == "number" then local now = Core:getFrameCount(); if type(now)=="number" then delta = now - (st.start_frame or 0) end end
        if st.click and st.dclick then
            if is_valid(st.stage) then
                st.stage:removeEventListener({ type = EventType.MouseClick,       listener = st.lc })
                st.stage:removeEventListener({ type = EventType.MouseDoubleClick, listener = st.ld })
            end
            st.phase = "done"
            return true
        end
        if delta > 60 then
            if not st.click then push("EventType_test4: MouseClick not observed within 60 frames") end
            if not st.dclick then push("EventType_test4: MouseDoubleClick not observed within 60 frames") end
            if is_valid(st.stage) then
                st.stage:removeEventListener({ type = EventType.MouseClick,       listener = st.lc })
                st.stage:removeEventListener({ type = EventType.MouseDoubleClick, listener = st.ld })
            end
            st.phase = "done"
            return true
        end
        return false
    end

    return true
end

-- EventType_test5: MouseEnter/MouseLeave on blueishBox
function M.EventType_test5()
    local st = S.t5
    if not st.init then
        st.init = true
        st.phase = "setup"
        st.frames = 0
    end

    if st.phase == "setup" then
        st.stage = get_stage(); if not is_valid(st.stage) then push("EventType_test5: mainStage missing"); return true end
        st.enter, st.leave = false, false
        st.le = function(_) st.enter = true end
        st.ll = function(_) st.leave = true end
        st.stage:addEventListener({ type = EventType.MouseEnter, listener = st.le })
        st.stage:addEventListener({ type = EventType.MouseLeave, listener = st.ll })
        if Core and Core.getFrameCount then local fc = Core:getFrameCount(); if type(fc)=="number" then st.start_frame = fc end end
        if Core and Core.pushMouseEvent then
            -- move outside, then inside blueishBox, then outside again
            Core:pushMouseEvent({ x = 10,  y = 10,  type = "move" })
            Core:pushMouseEvent({ x = 200, y = 100, type = "move" }) -- inside blueishBox
            Core:pushMouseEvent({ x = 10,  y = 10,  type = "move" }) -- leave
        end
        st.phase = "wait"
        return false
    end

    if st.phase == "wait" then
        st.frames = st.frames + 1
        local delta = st.frames
        if Core and Core.getFrameCount and type(st.start_frame) == "number" then local now = Core:getFrameCount(); if type(now)=="number" then delta = now - (st.start_frame or 0) end end
        if st.enter and st.leave then
            if is_valid(st.stage) then
                st.stage:removeEventListener({ type = EventType.MouseEnter, listener = st.le })
                st.stage:removeEventListener({ type = EventType.MouseLeave, listener = st.ll })
            end
            st.phase = "done"
            return true
        end
        if delta > 60 then
            if not st.enter then push("EventType_test5: MouseEnter not observed within 60 frames") end
            if not st.leave then push("EventType_test5: MouseLeave not observed within 60 frames") end
            if is_valid(st.stage) then
                st.stage:removeEventListener({ type = EventType.MouseEnter, listener = st.le })
                st.stage:removeEventListener({ type = EventType.MouseLeave, listener = st.ll })
            end
            st.phase = "done"
            return true
        end
        return false
    end

    return true
end

-- EventType_test1: Verify re-entrant machinery with a MouseMove event
function M.EventType_test1()
    local st = S.t1
    if not st.init then
        st.init = true
        st.phase = "setup"
        st.frames = 0
    end

    if st.phase == "setup" then
        st.stage = get_stage(); if not is_valid(st.stage) then push("EventType_test1: mainStage missing"); return true end
        st.move_hit = false
        st.listener = function(_) st.move_hit = true end
        st.stage:addEventListener({ type = EventType.MouseMove, listener = st.listener })
        -- capture frame baseline from Core
        if Core and Core.getFrameCount then
            local fc = Core:getFrameCount()
            if type(fc) == "number" then st.start_frame = fc end
        end
        if Core and Core.pushMouseEvent then
            Core:pushMouseEvent({ x = 8, y = 8, type = "move" })
        end
        st.phase = "wait"
        return false -- re-entrant: wait for dispatch
    end

    if st.phase == "wait" then
        st.frames = st.frames + 1
        local delta = st.frames
        if Core and Core.getFrameCount and type(st.start_frame) == "number" then
            local now = Core:getFrameCount()
            if type(now) == "number" then delta = now - (st.start_frame or 0) end
        end
        if st.move_hit then
            if is_valid(st.stage) then
                st.stage:removeEventListener({ type = EventType.MouseMove, listener = st.listener })
            end
            st.phase = "done"
            return true
        end
        if delta > 60 then
            push("EventType_test1: MouseMove did not dispatch within 60 frames")
            if is_valid(st.stage) then
                st.stage:removeEventListener({ type = EventType.MouseMove, listener = st.listener })
            end
            st.phase = "done"
            return true
        end
        return false
    end

    return true
end

-- EventType_test2: Keyboard KeyDown/KeyUp roundtrip via Core:pushKeyboardEvent
function M.EventType_test2()
    local st = S.t2
    if not st.init then
        st.init = true
        st.phase = "setup"
        st.frames = 0
    end

    if st.phase == "setup" then
        st.stage = get_stage(); if not is_valid(st.stage) then push("EventType_test2: mainStage missing"); return true end
        st.kd, st.ku = false, false
        -- listeners
        st.lkd = function(_) st.kd = true end
        st.lku = function(_) st.ku = true end
        st.stage:addEventListener({ type = EventType.KeyDown, listener = st.lkd })
        st.stage:addEventListener({ type = EventType.KeyUp,   listener = st.lku })
        -- baseline
        if Core and Core.getFrameCount then
            local fc = Core:getFrameCount(); if type(fc) == "number" then st.start_frame = fc end
        end
        -- push synthetic key down/up for an arbitrary key (65='A')
        if Core and Core.pushKeyboardEvent then
            Core:pushKeyboardEvent({ key = 65, type = "down" })
            Core:pushKeyboardEvent({ key = 65, type = "up" })
        end
        st.phase = "wait"
        return false
    end

    if st.phase == "wait" then
        st.frames = st.frames + 1
        local delta = st.frames
        if Core and Core.getFrameCount and type(st.start_frame) == "number" then
            local now = Core:getFrameCount(); if type(now) == "number" then delta = now - (st.start_frame or 0) end
        end
        if st.kd and st.ku then
            if is_valid(st.stage) then
                st.stage:removeEventListener({ type = EventType.KeyDown, listener = st.lkd })
                st.stage:removeEventListener({ type = EventType.KeyUp,   listener = st.lku })
            end
            st.phase = "done"
            return true
        end
        if delta > 60 then
            if not st.kd then push("EventType_test2: KeyDown not observed within 60 frames") end
            if not st.ku then push("EventType_test2: KeyUp not observed within 60 frames") end
            if is_valid(st.stage) then
                st.stage:removeEventListener({ type = EventType.KeyDown, listener = st.lkd })
                st.stage:removeEventListener({ type = EventType.KeyUp,   listener = st.lku })
            end
            st.phase = "done"
            return true
        end
        return false
    end

    return true
end

-- EventType_test3: Mouse event roundtrip (down, move, wheel, up)
function M.EventType_test3()
    local st = S.t3
    if not st.init then
        st.init = true
        st.phase = "setup"
        st.frames = 0
    end

    if st.phase == "setup" then
        st.stage = get_stage(); if not is_valid(st.stage) then push("EventType_test3: mainStage missing"); return true end
        st.down, st.up, st.move, st.wheel = false, false, false, false
        -- listeners
        st.ldown = function(_) st.down = true end
        st.lup   = function(_) st.up   = true end
        st.lmove = function(_) st.move = true end
        st.lwheel= function(_) st.wheel= true end
        st.stage:addEventListener({ type = EventType.MouseButtonDown, listener = st.ldown })
        st.stage:addEventListener({ type = EventType.MouseButtonUp,   listener = st.lup })
        st.stage:addEventListener({ type = EventType.MouseMove,       listener = st.lmove })
        st.stage:addEventListener({ type = EventType.MouseWheel,      listener = st.lwheel })
        -- baseline
        if Core and Core.getFrameCount then
            local fc = Core:getFrameCount(); if type(fc) == "number" then st.start_frame = fc end
        end
        -- push synthetic mouse events
        if Core and Core.pushMouseEvent then
            Core:pushMouseEvent({ x = 10, y = 10, type = "down", button = 1 })
            Core:pushMouseEvent({ x = 12, y = 11, type = "move" })
            Core:pushMouseEvent({ x = 12, y = 11, type = "wheel" })
            Core:pushMouseEvent({ x = 10, y = 10, type = "up",   button = 1 })
        end
        st.phase = "wait"
        return false
    end

    if st.phase == "wait" then
        st.frames = st.frames + 1
        local delta = st.frames
        if Core and Core.getFrameCount and type(st.start_frame) == "number" then
            local now = Core:getFrameCount(); if type(now) == "number" then delta = now - (st.start_frame or 0) end
        end
        if st.down and st.up and st.move and st.wheel then
            if is_valid(st.stage) then
                st.stage:removeEventListener({ type = EventType.MouseButtonDown, listener = st.ldown })
                st.stage:removeEventListener({ type = EventType.MouseButtonUp,   listener = st.lup })
                st.stage:removeEventListener({ type = EventType.MouseMove,       listener = st.lmove })
                st.stage:removeEventListener({ type = EventType.MouseWheel,      listener = st.lwheel })
            end
            st.phase = "done"
            return true
        end
        if delta > 60 then
            if not st.down  then push("EventType_test3: MouseButtonDown not observed within 60 frames") end
            if not st.move  then push("EventType_test3: MouseMove not observed within 60 frames") end
            if not st.wheel then push("EventType_test3: MouseWheel not observed within 60 frames") end
            if not st.up    then push("EventType_test3: MouseButtonUp not observed within 60 frames") end
            if is_valid(st.stage) then
                st.stage:removeEventListener({ type = EventType.MouseButtonDown, listener = st.ldown })
                st.stage:removeEventListener({ type = EventType.MouseButtonUp,   listener = st.lup })
                st.stage:removeEventListener({ type = EventType.MouseMove,       listener = st.lmove })
                st.stage:removeEventListener({ type = EventType.MouseWheel,      listener = st.lwheel })
            end
            st.phase = "done"
            return true
        end
        return false
    end

    return true
end

-- Step all re-entrant EventType tests; returns true when all complete
function M.step()
    local ok0 = M.EventType_test0()
    local ok1 = M.EventType_test1()
    local ok2 = M.EventType_test2()
    local ok3 = M.EventType_test3()
    local ok4 = M.EventType_test4()
    local ok5 = M.EventType_test5()
    local ok6 = M.EventType_test6()
    local ok7 = M.EventType_test7()
    local ok8 = M.EventType_test8()
    return ok0 and ok1 and ok2 and ok3 and ok4 and ok5 and ok6 and ok7 and ok8
end

-- One-shot (non re-entrant) sanity checks used by the C++ Lua test harness
local function oneshot_sanity()
    assert_true(EventType ~= nil, "EventType table missing in Lua environment")
    assert_true(type(EventType) == "table", "EventType is not a table")
    assert_true(EventType.Resize ~= nil, "EventType.Resize missing")
    assert_true(EventType.MouseMove ~= nil, "EventType.MouseMove missing")
end

-- Execute one-shot sanity tests (Lua harness is one-shot)
local _ok, _err = pcall(oneshot_sanity)
if not _ok then push("Lua test runner threw: " .. tostring(_err)) end

-- Return a table that works for both harness styles:
--  • C++ run_lua_tests expects { ok=bool, errors=table }
--  • callbacks.unittest requires this file and calls .step() each frame
local results = utils.get_results()
results.step = M.step
return results
