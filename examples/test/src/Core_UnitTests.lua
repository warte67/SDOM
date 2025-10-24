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



-- -- Test Core:getStage()
-- local stage = Core:getStage()
-- if not stage then
--     utils.push_error("Core:getStage() returned nil")
-- end

-- -- Test Core:getDisplayObject()
-- local stage2 = Core:getDisplayObject("stageTwo")
-- if not stage2 then
--     utils.push_error("Core:getDisplayObject('stageTwo') returned an invalid handle.")
-- end

-- -- Test setStage()
-- Core:setStage(stage2)
-- local root_obj = Core:getRoot()
-- if not root_obj then
--     utils.push_error("Core:getRoot() return an invalid handle.")
-- end
-- print("Current root object name: " .. root_obj:getName())
-- print("Expected root object name: " .. stage2:getName())
-- if root_obj:getName() ~= stage2:getName() then
--     utils.push_error("root_obj: " .. root_obj:getName() .. " is not 'stageTwo'.")
-- end
-- -- Test setRootNode()
-- Core:setRootNode("stageThree")




local function safe_get_name(obj)
    local ok, res = pcall(function() return obj:getName() end)
    if not ok then return nil, res end
    return res
end

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

-- Test Core:setStage() does not throw
local ok, err = pcall(function() Core:setStage(stage2) end)
if not ok then
    utils.push_error("Core:setStage(stage2) threw: " .. tostring(err))
end

-- Test Core:getRoot()
local root_obj = Core:getRoot()
if not root_obj then
    utils.push_error("Core:getRoot() returned an invalid handle.")
end

-- Safely obtain names (avoid runtime errors indexing userdata)
local root_name, root_err = safe_get_name(root_obj)
local stage2_name, stage2_err = safe_get_name(stage2)

if not root_name then
    utils.push_error("root_obj:getName() failed: " .. tostring(root_err))
end
if not stage2_name then
    utils.push_error("stage2:getName() failed: " .. tostring(stage2_err))
end

if root_name and stage2_name and root_name ~= stage2_name then
    utils.push_error("root_obj name '" .. tostring(root_name) .. "' does not match stageTwo name '" .. tostring(stage2_name) .. "'.")
end

-- Test Core:setRootNode() does not throw and changes root as expected
ok, err = pcall(function() Core:setRootNode("stageThree") end)
if not ok then
    utils.push_error("Core:setRootNode('stageThree') threw: " .. tostring(err))
else
    local new_root = Core:getRoot()
    if not new_root then
        utils.push_error("Core:getRoot() returned nil after setRootNode('stageThree').")
    else
        local new_root_name, nr_err = safe_get_name(new_root)
        if not new_root_name then
            utils.push_error("new_root:getName() failed: " .. tostring(nr_err))
        elseif new_root_name ~= "stageThree" then
            utils.push_error("Expected root name 'stageThree', got '" .. tostring(new_root_name) .. "'.")
        end
    end
end

-- Test Core:getDisplayObject() with invalid name should not return a valid handle
local invalid = Core:getDisplayObject("nonexistentStage")
if invalid then
    utils.push_error("Core:getDisplayObject('nonexistentStage') returned a handle; expected nil/invalid.")
end

-- Ensure stage and stageTwo are distinct handles (if both valid)
if stage and stage2 then
    if stage == stage2 then
        utils.push_error("Core:getStage() and Core:getDisplayObject('stageTwo') returned the same handle unexpectedly.")
    end
end

-- Restore the original stage
Core:setStage(stage)

-- getIsTraversing / setIsTraversing tests
local original_traversing = Core:getIsTraversing()
if (original_traversing == nil) then
    utils.push_error("Core:getIsTraversing() returned nil; expected boolean.")
else
    -- Toggle the traversing state
    Core:setIsTraversing(not original_traversing)
    local new_traversing = Core:getIsTraversing()
    if new_traversing == original_traversing then
        utils.push_error("Core:setIsTraversing() did not change the traversing state as expected.")
    end
    -- Restore original state
    Core:setIsTraversing(original_traversing)
    local restored_traversing = Core:getIsTraversing()
    if restored_traversing ~= original_traversing then
        utils.push_error("Core:setIsTraversing() did not restore the original traversing state.")
    end
end

-- createDisplayObject (Create an Orphan) Tests --
local boxInit = {
    name = "unitTestBox",
    type = "Box",
    x = 50,
    y = 50,
    width = 100,
    height = 100,
    color = {255, 0, 0, 255} -- Red box
}
local box = Core:createDisplayObject("Box", boxInit)
if not box or not box.isValid or not box:isValid() then
    utils.push_error("Core:createDisplayObject('Box', ...) returned an invalid handle.")
else
    if box:getName() ~= "unitTestBox" then
        utils.push_error("Created box name mismatch; expected 'unitTestBox', got '" .. tostring(box:getName()) .. "'.")
    end
end

-- Orphan Management Tests --

local orphan_count = Core:countOrphanedDisplayObjects()
if type(orphan_count) ~= "number" then
    utils.push_error("Core:countOrphanedDisplayObjects() did not return a number.")
end
if (orphan_count ~= 1) then
    utils.push_error("Core:countOrphanedDisplayObjects() Should be 1 orphan, found: " .. tostring(orphan_count))
end

-- -- Add the box to the stage to remove orphan status
-- stage:addChild(box)



-- return summary
return utils.get_results()