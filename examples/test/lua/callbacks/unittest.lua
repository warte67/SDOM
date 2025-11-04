-- luacheck: globals Core Core_UnitTests Factory_UnitTests IDisplayObject_UnitTests Stage_UnitTests Box_UnitTests LUA_UnitTests DomHandle_UnitTests
---@diagnostic disable: undefined-global
-- Callback module for running unit tests
local M = {}
local reentrant_evt = nil
do
    local ok, mod = pcall(require, "src.EventType_UnitTests")
    if ok and type(mod) == "table" and type(mod.step) == "function" then
        reentrant_evt = mod
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
    -- Step Lua re-entrant EventType tests (if module loaded)
    if reentrant_evt then
        local ok = reentrant_evt.step()
        allTestsPassed = allTestsPassed and ok
    end
    -- add more unit tests here as needed ...
    return allTestsPassed
end

return M
