-- Core_UnitTests.lua

-- require the helper
local utils = require("src.UnitTests")

-- Do not use print statements in unit test scripts, as they may interfere with test output.
    -- print("Core_UnitTests script entered successfully.")

-- Push Errors (Example errors; replace with actual test results) instead of print statements
    -- utils.push_error("widget X did not initialize")
    -- utils.push_error("asset Y missing")

-- Stage/Root Node Management 

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