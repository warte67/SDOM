-- scaffold_UnitTests.lua

-- Do not use print statements in unit test scripts, as they may interfere with test output.
    -- print("scaffold_UnitTests script entered successfully.")

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


-- Test 1: Lua Unit Test Function Scaffolding
local function test_scaffold()
    local stage = get_stage(); if not is_valid(stage) then return end
    local box = get_blueish(); if not is_valid(box) then return end
    local label = get_blueish_label(); if not is_valid(label) then return end

    -- Add Test Code Here

end


-- Run all tests sequentially
local function run_all()
    test_scaffold()

end

-- Execute
local ok, err = pcall(run_all)
if not ok then
    push("Lua test runner threw: " .. tostring(err))
end

-- return summary
return utils.get_results()