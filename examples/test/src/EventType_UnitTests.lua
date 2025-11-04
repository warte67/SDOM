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
}

-- EventType_test0: Placeholder template (immediate success)
function M.EventType_test0()
    return true -- completed
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
    return ok0 and ok1 and ok2 and ok3 and ok4 and ok5
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
