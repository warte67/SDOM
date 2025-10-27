-- Core_UnitTests.lua (structured like IDisplayObject_UnitTests.lua)

-- require the helper
local utils = require("src.UnitTests")

-- Small helpers
local function push(msg) utils.push_error(msg) end
local function assert_true(cond, msg) if not cond then push(msg) end end
local function is_valid(h) return (h ~= nil) and (h.isValid == nil or h:isValid()) end

-- Common object accessors (names configured in config.lua)
local function get_stage1()
    local s = Core:getDisplayObject("mainStage") or Core:getStage()
    if not is_valid(s) then push("mainStage not found") end
    return s
end
local function get_stage2()
    local s = Core:getDisplayObject("stageTwo")
    if not is_valid(s) then push("stageTwo not found") end
    return s
end

-- Helpers
local function safe_get_name(obj)
    local ok, res = pcall(function() return obj:getName() end)
    if not ok then return nil, res end
    return res
end

-- Test 1: Stage/Root management (get/set, swap, restore)
local function test_stage_root()
    local stageA = get_stage1(); if not is_valid(stageA) then return end
    local stageB = get_stage2(); if not is_valid(stageB) then return end

    -- setStage should not throw
    local ok, err = pcall(function() Core:setStage(stageB) end)
    assert_true(ok, "Core:setStage(stageTwo) threw: " .. tostring(err))

    -- root should match stageB
    local root = Core:getRoot(); assert_true(is_valid(root), "Core:getRoot() invalid after setStage")
    local rname = root and root.getName and root:getName() or nil
    local bname = stageB and stageB.getName and stageB:getName() or nil
    assert_true(rname ~= nil and bname ~= nil and rname == bname,
        "Root name '" .. tostring(rname) .. "' != stageTwo '" .. tostring(bname) .. "'")

    -- setRootNode() to stageThree (if present) should not throw
    ok, err = pcall(function() Core:setRootNode("stageThree") end)
    assert_true(ok, "Core:setRootNode('stageThree') threw: " .. tostring(err))
    local new_root = Core:getRoot(); assert_true(is_valid(new_root), "Core:getRoot() invalid after setRootNode")

    -- Restore original stage
    Core:setStage(stageA)
end

-- Test 2: Traversal flag toggle/restore
local function test_traversing_flag()
    local original = Core:getIsTraversing()
    assert_true(type(original) == "boolean", "Core:getIsTraversing() did not return boolean")
    Core:setIsTraversing(not original)
    local toggled = Core:getIsTraversing()
    assert_true(toggled ~= original, "Core:setIsTraversing() did not toggle")
    Core:setIsTraversing(original)
    local restored = Core:getIsTraversing()
    assert_true(restored == original, "Core:setIsTraversing() did not restore original")
end

-- Test 3: Creation + Orphan management
local function test_creation_orphans()
    local stage = get_stage1(); if not is_valid(stage) then return end
    local init = { name = "unitTestBox", type = "Box", x = 50, y = 50, width = 100, height = 100, color = {255,0,0,255} }
    local h = Core:createDisplayObject("Box", init)
    assert_true(is_valid(h), "Core:createDisplayObject returned invalid handle")
    if is_valid(h) then assert_true(h:getName() == "unitTestBox", "Created name mismatch") end

    local count = Core:countOrphanedDisplayObjects()
    assert_true(type(count) == "number", "countOrphanedDisplayObjects did not return a number")
    assert_true(count == 1, "Expected 1 orphan after creation; got " .. tostring(count))

    stage:addChild(h)
    count = Core:countOrphanedDisplayObjects(); assert_true(count == 0, "Expected 0 orphans after addChild; got " .. tostring(count))

    stage:removeChild(h)
    count = Core:countOrphanedDisplayObjects(); assert_true(count == 1, "Expected 1 orphan after removeChild; got " .. tostring(count))

    Core:collectGarbage()
    count = Core:countOrphanedDisplayObjects(); assert_true(count == 0, "Expected 0 orphans after collectGarbage; got " .. tostring(count))
end

-- Test 4: Lookup helpers (valid/invalid)
local function test_lookup()
    local s1 = get_stage1(); local s2 = get_stage2()
    if is_valid(s1) and is_valid(s2) then
        assert_true(s1 ~= s2, "getStage and getDisplayObject('stageTwo') returned same handle")
    end
    local bad = Core:getDisplayObject("nonexistentStage")
    assert_true(bad == nil or (bad.isValid and not bad:isValid()), "getDisplayObject(nonexistent) should be nil/invalid")
end

-- Test 5: Basic config accessors return sane types
local function test_config_accessors()
    local w = Core:getPixelWidth(); local h = Core:getPixelHeight()
    assert_true(type(w) == "number" and type(h) == "number" and w > 0 and h > 0, "Pixel size accessors invalid")
    local fmt = Core:getPixelFormat(); assert_true(type(fmt) == "number" or type(fmt) == "string", "Pixel format invalid")
    local flags = Core:getWindowFlags(); assert_true(type(flags) == "number", "Window flags invalid")
end

-- Runner
local function run_all()
    test_stage_root()
    test_traversing_flag()
    test_creation_orphans()
    test_lookup()
    test_config_accessors()
end

local ok, err = pcall(run_all)
if not ok then push("Core Lua test runner threw: " .. tostring(err)) end

return utils.get_results()
