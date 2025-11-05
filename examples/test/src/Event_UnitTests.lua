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
local function assert_true(cond, msg)
    if not cond then push(msg) end
end
local function assert_false(cond, msg)
    if cond then push(msg) end
end
---@param h DisplayHandle|nil
---@return boolean h_is_valid
local function is_valid(h)
    return (h ~= nil) and (h.isValid == nil or h:isValid())
end


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

-- 

-- Test 1: Verify EventType table is registered and accessible
local function test_table_accessible()
    assert_true(EventType ~= nil, "EventType table missing in Lua environment")
    assert_true(type(EventType) == "table", "EventType is not a table")
end


--[[



-- Test 2: Verify that known EventTypes exist
local function test_all_predefined_eventtypes()
    local required = {
        "None", "SDL_Event", "Quit", "EnterFrame",
        "MouseButtonUp", "MouseButtonDown", "MouseWheel",
        "MouseMove", "MouseClick", "MouseDoubleClick",
        "MouseEnter", "MouseLeave",
        "StageClosed", "KeyDown", "KeyUp",
        "FocusGained", "FocusLost", "Resize", "Move",
        "Show", "Hide", "EnterFullscreen", "LeaveFullscreen",
        "ValueChanged", "StateChanged", "SelectionChanged",
        "Enabled", "Disabled", "Visible", "Hidden",
        "Drag", "Dragging", "Drop",
        "ClipboardCopy", "ClipboardPaste",
        "Added", "Removed", "AddedToStage", "RemovedFromStage",
        "OnInit", "OnQuit", "OnEvent", "OnUpdate", "OnRender", "OnPreRender",
        "User"
    }

    for _, name in ipairs(required) do
        local et = EventType[name]
        assert_true(et ~= nil, "Missing EventType: " .. name)
        assert_true(type(et) == "userdata", "EventType." .. name .. " is not userdata")
    end
end

-- Test 3: Verify properties on a few key EventTypes
local function test_eventtype_properties()
    local function verify(name, exp)
        local et = EventType[name]
        if not et then return push("EventType " .. name .. " missing") end
        if et.captures ~= exp.captures then
            push(string.format("%s.captures mismatch: got %s, expected %s", name, tostring(et.captures), tostring(exp.captures)))
        end
        if et.bubbles ~= exp.bubbles then
            push(string.format("%s.bubbles mismatch: got %s, expected %s", name, tostring(et.bubbles), tostring(exp.bubbles)))
        end
        if et.targetOnly ~= exp.targetOnly then
            push(string.format("%s.targetOnly mismatch: got %s, expected %s", name, tostring(et.targetOnly), tostring(exp.targetOnly)))
        end
        if et.global ~= exp.global then
            push(string.format("%s.global mismatch: got %s, expected %s", name, tostring(et.global), tostring(exp.global)))
        end
    end

    verify("Quit",        {captures=false, bubbles=false, targetOnly=false, global=true})
    verify("MouseMove",   {captures=false, bubbles=false, targetOnly=true,  global=false})
    verify("Resize",      {captures=true,  bubbles=true,  targetOnly=false, global=false})
    verify("OnQuit",      {captures=false, bubbles=false, targetOnly=false, global=true})
end

-- Test 4: Callable lookup test
local function test_callable_lookup()
    local et = EventType("MouseClick")
    assert_true(et ~= nil, "EventType('MouseClick') returned nil")
    assert_true(et.name == "MouseClick", "EventType('MouseClick') name mismatch")
end

-- Test 5: Runtime registration
local function test_runtime_registration()
    local name = "CustomEventTypeUnitTest"
    local newType = EventType.register(name, true, true, false, false)
    assert_true(newType ~= nil, "EventType.register() returned nil")
    assert_true(EventType[name] ~= nil, "Newly registered event missing from Lua table")
    assert_true(newType.name == name, "Registered EventType name mismatch")
end
--]]

local function run_all()
    test_table_accessible()
    -- test_all_predefined_eventtypes()
    -- test_eventtype_properties()
    -- test_callable_lookup()
    -- test_runtime_registration()
end

-- Re-entrant wrapper: expose step() that runs once and then returns true
local M = { _done = false }
function M.step()
    if M._done then return true end
    local ok, err = pcall(run_all)
    if not ok then push("Lua test runner threw: " .. tostring(err)) end
    M._done = true
    return true
end

local results = utils.get_results()
results.step = M.step
return results
