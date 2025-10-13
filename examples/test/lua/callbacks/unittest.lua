-- luacheck: globals Core Core_UnitTests Factory_UnitTests IDisplayObject_UnitTests Stage_UnitTests Box_UnitTests LUA_UnitTests DomHandle_UnitTests
---@diagnostic disable: undefined-global
-- Callback module for running unit tests
local M = {}

function M.on_unit_test()
    local allTestsPassed = true
    allTestsPassed = allTestsPassed and Core_UnitTests()
    allTestsPassed = allTestsPassed and Factory_UnitTests()
    allTestsPassed = allTestsPassed and IDisplayObject_UnitTests()
    allTestsPassed = allTestsPassed and Stage_UnitTests()
    allTestsPassed = allTestsPassed and Box_UnitTests()
    allTestsPassed = allTestsPassed and LUA_UnitTests()
    allTestsPassed = allTestsPassed and DisplayHandle_UnitTests()
    allTestsPassed = allTestsPassed and EventType_UnitTests()
    allTestsPassed = allTestsPassed and Event_UnitTests()
    allTestsPassed = allTestsPassed and GarbageCollection_UnitTests()
    allTestsPassed = allTestsPassed and SpriteSheet_UnitTests()
    allTestsPassed = allTestsPassed and Label_UnitTests()
    return allTestsPassed
end

return M
