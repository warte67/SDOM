-- Core_UnitTests.lua

-- require the helper
local utils = require("src.UnitTests")

-- Do not use print statements in unit test scripts, as they may interfere with test output. 
-- Unless debugging, use utils.push_error() to log errors.
    -- print("Core_UnitTests script entered successfully.")

-- Push Errors (Example errors; replace with actual test results) instead of print statements
    -- utils.push_error("widget X did not initialize")
    -- utils.push_error("asset Y missing")

-- Stage/Root Node Management 

-- -- Diagnostic: record runtime shape of Core and its getStage entry
-- do
--     local ok, msg = pcall(function()
--         print("DEBUG: type(Core) = " .. tostring(type(Core)))
--         print("DEBUG: tostring(Core) = " .. tostring(Core))
--         local g = Core.getStage
--         print("DEBUG: type(Core.getStage) = " .. tostring(type(g)))
--         if g then
--             print("DEBUG: Core.getStage is function? " .. tostring(type(g) == 'function'))
--         end
--     end)
--     if not ok then
--         print("DEBUG: failed to introspect Core: " .. tostring(msg))
--     end
-- end



local stage = Core:getStage()
if not stage then
    utils.push_error("Core.getStage() returned nil")
else
    -- Verify stage properties
    if stage:getWidth() <= 0 then
        utils.push_error("Stage width is not greater than 0")
    end
    if stage:getHeight() <= 0 then
        utils.push_error("Stage height is not greater than 0")
    end
end



-- return summary
return utils.get_results()