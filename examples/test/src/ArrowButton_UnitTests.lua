-- ArrowButton_UnitTests.lua
-- Lua re-entrant unit tests for ArrowButton-specific API.
-- Do not use print statements; push errors via utils.push_error().

local utils = require("src.UnitTests")
local function push(msg) utils.push_error(msg) end
local function assert_true(cond, msg) if not cond then push(msg) end end
local function is_valid(h) return (h ~= nil) and (h.isValid == nil or h:isValid()) end

local M = {}
local S = {
    t1 = { init=false, phase="init", frames=0, btn=nil, stage=nil, start_frame=0 },
    t2 = { init=false, phase="init", frames=0, btn=nil, start_frame=0 },
    t3 = { init=false, phase="init", frames=0, btn=nil, stage=nil },
}

-- ArrowButton_test0: placeholder sanity
function M.ArrowButton_test0()
    return true
end

-- ArrowButton_test1: Validate creation + direction property roundtrip
function M.ArrowButton_test1()
    local st = S.t1
    if not st.init then
        st.init, st.phase, st.frames = true, "setup", 0
    end

    if st.phase == "setup" then
        st.stage = Core:getDisplayObject("mainStage")
        if not is_valid(st.stage) then push("ArrowButton_test1: mainStage missing") return true end

        -- create a temporary arrow button
        st.btn = Core:createDisplayObject("ArrowButton", {
            name="lua_arrow_test", type="ArrowButton", x=40, y=360, width=32, height=32, direction="up"
        })
        if not is_valid(st.btn) then push("ArrowButton_test1: failed to create ArrowButton") return true end
        st.stage:addChild(st.btn)
        st.phase = "validate"
        return false
    end

    if st.phase == "validate" then
        local btn = st.btn --[[@as ArrowButton]]
        local dir = btn:getDirection()
        if dir ~= "up" and dir ~= ArrowDirection.Up then
            push("ArrowButton_test1: expected direction='up', got "..tostring(dir))
        end

        -- mutate via setter
        btn:setDirection("left")
        local d2 = btn:getDirection()
        if d2 ~= "left" and d2 ~= ArrowDirection.Left then
            push("ArrowButton_test1: setDirection('left') failed, got "..tostring(d2))
        end

        st.phase = "cleanup"
        return false
    end

    if st.phase == "cleanup" then
        if is_valid(st.stage) and is_valid(st.btn) then
            st.stage:removeChild(st.btn)
            Core:destroyDisplayObject("lua_arrow_test")
        end
        st.phase = "done"
        return true
    end

    return true
end

-- ArrowButton_test2: Verify arrow state changes with MouseDown/MouseUp
function M.ArrowButton_test2()
    local st = S.t2
    if not st.init then
        st.init, st.phase, st.frames = true, "setup", 0
    end

    if st.phase == "setup" then
        st.btn = Core:getDisplayObject("main_stage_arrowbutton")
        if not is_valid(st.btn) then
            push("ArrowButton_test2: main_stage_arrowbutton missing in config")
            return true
        end

        st.start_frame = Core.getFrameCount and Core:getFrameCount() or 0

        local left, top = st.btn:getLeft(), st.btn:getTop()
        local w, h = st.btn:getWidth(), st.btn:getHeight()
        st.cx, st.cy = left + math.floor(w/2), top + math.floor(h/2)

        -- push down/up sequence
        if Core and Core.pushMouseEvent then
            Core:pushMouseEvent({ x=st.cx, y=st.cy, type="down", button=1 })
            Core:pushMouseEvent({ x=st.cx, y=st.cy, type="up",   button=1 })
        end

        st.phase = "wait"
        return false
    end

    if st.phase == "wait" then
        st.frames = st.frames + 1
        -- IDE hint: st.btn is a DisplayHandle; cast for ArrowButton-specific methods
        local btn = st.btn --[[@as ArrowButton]]
        local s = btn:getArrowState()
        if s == "raised" or s == ArrowState.Raised then
            st.phase = "done"
            return true
        end
        if st.frames > 60 then
            push("ArrowButton_test2: Arrow state did not return to Raised after click")
            st.phase = "done"
            return true
        end
        return false
    end

    return true
end

-- ArrowButton_test3: Validate getIconIndex across direction/state changes
-- Exercises API:
--  - getDirection / setDirection
--  - getArrowState
--  - getIconIndex
function M.ArrowButton_test3()
    local st = S.t3
    if not st.init then
        st.init, st.phase, st.frames = true, "setup", 0
    end

    if st.phase == "setup" then
        st.stage = Core:getDisplayObject("mainStage")
        if not is_valid(st.stage) then push("ArrowButton_test3: mainStage missing") return true end

        -- create a temporary arrow button
        -- Place it in a clear area to avoid overlap with existing UI.
        -- Note: mainStage has buttons near y=370; keep y small and x < rightMainFrame (x<300).
        st.btn = Core:createDisplayObject("ArrowButton", {
            name="lua_arrow_icon_test", type="ArrowButton",
            x=220, y=8, width=16, height=16,
            direction="up",
            icon_resource = "internal_icon_16x16"
        })
        if not is_valid(st.btn) then push("ArrowButton_test3: failed to create ArrowButton") return true end
        st.stage:addChild(st.btn)

        -- baseline icon index (should be numeric)
        local btn = st.btn --[[@as ArrowButton]]
        st.i0 = btn:getIconIndex()
        if type(st.i0) ~= "number" then
            push("ArrowButton_test3: getIconIndex() did not return a number, got "..tostring(st.i0))
        end

        st.phase = "validate_dir"
        return false
    end

    if st.phase == "validate_dir" then
        -- change direction and verify icon index changes
        local btn = st.btn --[[@as ArrowButton]]
        btn:setDirection("left")
        local d = btn:getDirection()
        if d ~= "left" and d ~= ArrowDirection.Left then
            push("ArrowButton_test3: setDirection('left') failed, got "..tostring(d))
        end

        st.i1 = btn:getIconIndex()
        if st.i1 == st.i0 then
            push("ArrowButton_test3: getIconIndex() did not change after direction change")
        end

        -- cache center for input events
        local left, top = st.btn:getLeft(), st.btn:getTop()
        local w, h = st.btn:getWidth(), st.btn:getHeight()
        st.cx, st.cy = left + math.floor(w/2), top + math.floor(h/2)

        -- hover motion then press to move to Depressed state
        if Core and Core.pushMouseEvent then
            Core:pushMouseEvent({ x=st.cx, y=st.cy, type="motion" })
            Core:pushMouseEvent({ x=st.cx, y=st.cy, type="down", button=1 })
        end

        st.frames = 0
        st.phase = "wait_depressed"
        return false
    end

    if st.phase == "wait_depressed" then
        st.frames = st.frames + 1
        local btn = st.btn --[[@as ArrowButton]]
        local s = btn:getArrowState()
        if s == "depressed" or s == ArrowState.Depressed then
            st.i2 = btn:getIconIndex()
            if st.i2 == st.i1 then
                push("ArrowButton_test3: getIconIndex() did not change when depressed")
            end
            -- release to return to Raised
            if Core and Core.pushMouseEvent then
                Core:pushMouseEvent({ x=st.cx, y=st.cy, type="up", button=1 })
            end
            st.frames = 0
            st.phase = "wait_raised"
            return false
        end
        if st.frames > 60 then
            push("ArrowButton_test3: Arrow did not reach Depressed state after down")
            -- attempt release anyway
            if Core and Core.pushMouseEvent then
                Core:pushMouseEvent({ x=st.cx, y=st.cy, type="up", button=1 })
            end
            st.frames = 0
            st.phase = "wait_raised"
            return false
        end
        return false
    end

    if st.phase == "wait_raised" then
        st.frames = st.frames + 1
        local btn = st.btn --[[@as ArrowButton]]
        local s = btn:getArrowState()
        if s == "raised" or s == ArrowState.Raised then
            local btn = st.btn --[[@as ArrowButton]]
            local i3 = btn:getIconIndex()
            if i3 ~= st.i1 then
                push("ArrowButton_test3: Icon index after release does not match raised state for direction")
            end
            st.phase = "cleanup"
            return false
        end
        if st.frames > 60 then
            push("ArrowButton_test3: Arrow did not return to Raised after release")
            st.phase = "cleanup"
            return false
        end
        return false
    end

    if st.phase == "cleanup" then
        if is_valid(st.stage) and is_valid(st.btn) then
            st.stage:removeChild(st.btn)
            Core:destroyDisplayObject("lua_arrow_icon_test")
        end
        st.phase = "done"
        return true
    end

    return true
end

-- step aggregator
function M.step()
    local ok0 = M.ArrowButton_test0()
    local ok1 = M.ArrowButton_test1()
    local ok2 = M.ArrowButton_test2()
    local ok3 = M.ArrowButton_test3()
    return ok0 and ok1 and ok2 and ok3
end

-- one-shot sanity for Lua environment
local function oneshot_sanity()
    assert_true(Core ~= nil, "Core missing")
    assert_true(Core.createDisplayObject ~= nil, "Core.createDisplayObject missing")
    assert_true(ArrowDirection ~= nil, "ArrowDirection enum missing")
end
local _ok, _err = pcall(oneshot_sanity)
if not _ok then push("ArrowButton oneshot sanity threw: " .. tostring(_err)) end

local results = utils.get_results()
results.step = M.step
return results
