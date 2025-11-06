-- Button_UnitTests.lua
---@diagnostic disable: undefined-global

local utils = require("src.UnitTests")
local function push(msg) utils.push_error(msg) end
local function assert_true(cond, msg) if not cond then push(msg) end end

local function run_all()
    -- Fetch a configured Button from the scene
    local h = Core:getDisplayObject("main_stage_button")
    assert_true(h and (h.isValid == nil or h:isValid()), "main_stage_button not found")
    if not (h and (h.isValid == nil or h:isValid())) then return end

    -- getText/setText
    local oldText = h:getText()
    h:setText("Lua UnitTest Button")
    assert_true(h:getText() == "Lua UnitTest Button", "setText/getText mismatch")
    -- restore
    h:setText(oldText)

    -- getLabelColor/setLabelColor
    local c0 = h:getLabelColor()
    h:setLabelColor({ r = 1, g = 2, b = 3, a = 4 })
    local c1 = h:getLabelColor()
    assert_true(c1.r == 1 and c1.g == 2 and c1.b == 3 and c1.a == 4, "setLabelColor/getLabelColor mismatch")
    -- restore
    if type(c0) == "table" then h:setLabelColor(c0) end

    -- getFontResource should be a non-empty string (matches config)
    local fr = h:getFontResource()
    assert_true(type(fr) == "string" and #fr > 0, "getFontResource returned empty")

    -- getLabelObject returns a DisplayHandle and should be valid
    local lh = h:getLabelObject()
    assert_true(lh and (lh.isValid == nil or lh:isValid()), "getLabelObject returned invalid handle")
end

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

