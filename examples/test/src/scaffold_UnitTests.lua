-- Core_UnitTests.lua

-- require the helper
local utils = require("src.UnitTests")

-- Do not use print statements in unit test scripts, as they may interfere with test output.
    -- print("Core_UnitTests script entered successfully.")

-- Push Errors (Example errors; replace with actual test results) instead of print statements
    -- utils.push_error("widget X did not initialize")
    -- utils.push_error("asset Y missing")



-- return summary
return utils.get_results()