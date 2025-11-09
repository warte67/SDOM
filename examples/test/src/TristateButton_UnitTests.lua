-- TristateButton_UnitTests.lua
---@diagnostic disable: undefined-global

local utils = require("src.UnitTests")
local function push(msg) utils.push_error(msg) end
local function assert_true(cond, msg) if not cond then push(msg) end end

local function is_color(t)
    return type(t) == "table"
        and type(t.r) == "number" and type(t.g) == "number"
        and type(t.b) == "number" and type(t.a) == "number"
end

local function in_range_rgba(c)
    return c.r >= 0 and c.r <= 255
       and c.g >= 0 and c.g <= 255
       and c.b >= 0 and c.b <= 255
       and c.a >= 0 and c.a <= 255
end

local function run_all()
    -- Fetch a configured TristateButton from the scene
    local h = Core:getDisplayObject("mainFrame_tristate_1")
    assert_true(h and (h.isValid == nil or h:isValid()), "mainFrame_tristate_1 not found")
    if not (h and (h.isValid == nil or h:isValid())) then return end

--     -- getText / setText
--     local oldText = h:getText()
-- print("oldText='" .. oldText .. "'")
--     h:setText("Lua UnitTest Tristate")
--     assert_true(h:getText() == "Lua UnitTest Tristate", "setText/getText mismatch (TristateButton)")
--     -- restore
--     h:setText(oldText)

    -- -- getLabelObject returns a DisplayHandle and should be valid
    -- local lh = h:getLabelObject()
    -- assert_true(lh and (lh.isValid == nil or lh:isValid()), "getLabelObject returned invalid handle")

    -- -- getIconButtonObject returns a DisplayHandle and should be valid
    -- local ibh = h:getIconButtonObject()
    -- assert_true(ibh and (ibh.isValid == nil or ibh:isValid()), "getIconButtonObject returned invalid handle")

    -- -- resources should be non-empty strings
    -- local fr = h:getFontResource()
    -- local ir = h:getIconResource()
    -- assert_true(type(fr) == "string" and #fr > 0, "getFontResource returned empty")
    -- assert_true(type(ir) == "string" and #ir > 0, "getIconResource returned empty")

    -- -- font metrics should be positive
    -- assert_true(type(h:getFontSize()) == "number" and h:getFontSize() > 0, "getFontSize not positive")
    -- assert_true(type(h:getFontWidth()) == "number" and h:getFontWidth() > 0, "getFontWidth not positive")
    -- assert_true(type(h:getFontHeight()) == "number" and h:getFontHeight() > 0, "getFontHeight not positive")

    -- -- icon metrics should be positive
    -- assert_true(type(h:getIconWidth()) == "number" and h:getIconWidth() > 0, "getIconWidth not positive")
    -- assert_true(type(h:getIconHeight()) == "number" and h:getIconHeight() > 0, "getIconHeight not positive")

    -- -- colors should be well-formed RGBA tables
    -- local lc = h:getLabelColor()
    -- local bc = h:getBorderColor()

    -- -- Debugging output to verify what Lua actually received
    -- print("[TristateButton_UnitTests.lua] getLabelColor():",
    --     "type(lc) =", type(lc),
    --     "r =", tostring(lc and lc.r),
    --     "g =", tostring(lc and lc.g),
    --     "b =", tostring(lc and lc.b),
    --     "a =", tostring(lc and lc.a)
    -- )
    -- print("[TristateButton_UnitTests.lua] getBorderColor():",
    --     "type(bc) =", type(bc),
    --     "r =", tostring(bc and bc.r),
    --     "g =", tostring(bc and bc.g),
    --     "b =", tostring(bc and bc.b),
    --     "a =", tostring(bc and bc.a)
    -- )

    -- assert_true(is_color(lc) and in_range_rgba(lc), "getLabelColor invalid range")
    -- assert_true(is_color(bc) and in_range_rgba(bc), "getBorderColor invalid range")

    -- -- getUseBorder should be boolean (value is config-dependent)
    -- local ub = h:getUseBorder()
    -- assert_true(type(ub) == "boolean", "getUseBorder did not return boolean")

    -- -- getIconIndex should be a number
    -- local idx = h:getIconIndex()
    -- assert_true(type(idx) == "number", "getIconIndex did not return a number")
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

