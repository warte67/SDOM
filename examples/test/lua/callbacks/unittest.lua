-- luacheck: globals Core Core_UnitTests Factory_UnitTests IDisplayObject_UnitTests Stage_UnitTests Box_UnitTests LUA_UnitTests DomHandle_UnitTests
---@diagnostic disable: undefined-global
-- Callback module for running unit tests
local M = {}

-- Allow the embedding runtime to disable running Lua-driven unit tests by
-- setting the global `SDOM_RUN_LUA_UNIT_TESTS = false` before this module
-- is loaded. This makes it possible for the C++ harness to take control of
-- test execution without modifying the Lua test files themselves.
if _G.SDOM_RUN_LUA_UNIT_TESTS == false then
    return { on_unit_test = function() return true end }
end

function M.on_unit_test()
    local allTestsPassed = true

    if true then       
        allTestsPassed = allTestsPassed and Core_UnitTests()
        allTestsPassed = allTestsPassed and IDisplayObject_UnitTests()
        allTestsPassed = allTestsPassed and Event_UnitTests()
        allTestsPassed = allTestsPassed and EventType_UnitTests()
        allTestsPassed = allTestsPassed and ArrowButton_UnitTests()
        allTestsPassed = allTestsPassed and AssetHandle_UnitTests()
        allTestsPassed = allTestsPassed and BitmapFont_UnitTests()
        allTestsPassed = allTestsPassed and Button_UnitTests()

        allTestsPassed = allTestsPassed and TristateButton_UnitTests()
        allTestsPassed = allTestsPassed and Variant_UnitTests()
        allTestsPassed = allTestsPassed and DataRegistry_UnitTests()
    end
    -- Re-entrant Lua tests are stepped by the C++ UnitTests runner now.
    -- No direct stepping here to avoid double-advancing state per frame.
    -- add more unit tests here as needed ...
    return allTestsPassed
end

return M
