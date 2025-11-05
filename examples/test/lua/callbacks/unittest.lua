-- luacheck: globals Core Core_UnitTests Factory_UnitTests IDisplayObject_UnitTests Stage_UnitTests Box_UnitTests LUA_UnitTests DomHandle_UnitTests
---@diagnostic disable: undefined-global
-- Callback module for running unit tests
local M = {}
local reentrant_evt = nil
local reentrant_core = nil
local reentrant_ido = nil
local reentrant_event = nil
do
    local ok, mod = pcall(require, "src.EventType_UnitTests")
    if ok and type(mod) == "table" and type(mod.step) == "function" then
        reentrant_evt = mod
    end
    ok, mod = pcall(require, "src.Core_UnitTests")
    if ok and type(mod) == "table" and type(mod.step) == "function" then
        reentrant_core = mod
    end
    ok, mod = pcall(require, "src.IDisplayObject_UnitTests")
    if ok and type(mod) == "table" and type(mod.step) == "function" then
        reentrant_ido = mod
    end
    ok, mod = pcall(require, "src.Event_UnitTests")
    if ok and type(mod) == "table" and type(mod.step) == "function" then
        reentrant_event = mod
    end
end

function M.on_unit_test()
    local allTestsPassed = true

    if true then       
        allTestsPassed = allTestsPassed and Core_UnitTests()
        allTestsPassed = allTestsPassed and IDisplayObject_UnitTests()
        allTestsPassed = allTestsPassed and Event_UnitTests()
        allTestsPassed = allTestsPassed and EventType_UnitTests()
    end
    -- Step Lua re-entrant tests (if modules loaded)
    local function step_mod(m)
        if m and type(m.step) == "function" then
            local ok = false
            local okc, err = pcall(function() ok = m.step() end)
            if not okc then ok = false end
            return ok
        end
        return true
    end
    allTestsPassed = allTestsPassed and step_mod(reentrant_core)
    allTestsPassed = allTestsPassed and step_mod(reentrant_ido)
    allTestsPassed = allTestsPassed and step_mod(reentrant_event)
    allTestsPassed = allTestsPassed and step_mod(reentrant_evt)
    -- add more unit tests here as needed ...
    return allTestsPassed
end

return M
