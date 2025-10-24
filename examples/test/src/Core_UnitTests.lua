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



-- Test Core:getStage()
local stage = Core:getStage()
if not stage then
    utils.push_error("Core:getStage() returned nil")
end

-- Test Core:getDisplayObject()
local stage2 = Core:getDisplayObject("stageTwo")
if not stage2 then
    utils.push_error("Core:getDisplayObject('stageTwo') returned an invalid handle.")
end

-- Test setStage()
Core:setStage(stage2)
local root_obj = Core:getRoot()
if not root_obj then
    utils.push_error("Core:getRoot() return an invalid handle.")
end
if root_obj ~= stage2 then
    utils.push_error("root_obj: " .. root_obj:getName() .. " is not 'stageTwo'.")
end
-- Test setRootNode()
Core:setRootNode("stageThree")



-- return summary
return utils.get_results()