-- luacheck: globals Core Core_UnitTests Factory_UnitTests IDisplayObject_UnitTests Stage_UnitTests Box_UnitTests LUA_UnitTests DomHandle_UnitTests
---@diagnostic disable: undefined-global
-- Callback module for running unit tests
local M = {}

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
    end
    -- Re-entrant Lua tests are stepped by the C++ UnitTests runner now.
    -- No direct stepping here to avoid double-advancing state per frame.
    -- add more unit tests here as needed ...
    return allTestsPassed
end

return M
