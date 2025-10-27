-- Core_UnitTests.lua (structured like IDisplayObject_UnitTests.lua)

-- Legend:
-- ✅ Test Verified
-- 🔄 In Progress
-- ⚠️ Failing
-- ☐ Planned

-- require the helper
local utils = require("src.UnitTests")

-- Editor/linter hints for engine-provided globals
-- luacheck: globals Core SDL_Utils
---@diagnostic disable: undefined-global

-- Local alias (avoids undefined-global when stubs aren't loaded by the IDE)
local SDL_Utils = rawget(_G, "SDL_Utils")

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

    -- ✅ Core:setStage() should not throw
    local ok, err = pcall(function() Core:setStage(stageB) end)
    assert_true(ok, "Core:setStage(stageTwo) threw: " .. tostring(err))

    -- ✅ Core:getRoot() — root should match stageB  
    local root = Core:getRoot(); assert_true(is_valid(root), "Core:getRoot() invalid after setStage")
    local rname = root and root.getName and root:getName() or nil
    local bname = stageB and stageB.getName and stageB:getName() or nil
    assert_true(rname ~= nil and bname ~= nil and rname == bname,
        "Root name '" .. tostring(rname) .. "' != stageTwo '" .. tostring(bname) .. "'")

    -- ✅ Core:setRootNode() to stageThree (if present) should not throw
    ok, err = pcall(function() Core:setRootNode("stageThree") end)
    assert_true(ok, "Core:setRootNode('stageThree') threw: " .. tostring(err))
    local new_root = Core:getRoot(); assert_true(is_valid(new_root), "Core:getRoot() invalid after setRootNode")

    -- ✅ Core:setStage() — restore original stage
    Core:setStage(stageA)
end

-- Test 2: Traversal flag toggle/restore
local function test_traversing_flag()
    -- ✅ Core:getIsTraversing()
    local original = Core:getIsTraversing()
    assert_true(type(original) == "boolean", "Core:getIsTraversing() did not return boolean")
    -- ✅ Core:setIsTraversing()
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
    -- ✅ Core:createDisplayObject()
    local h = Core:createDisplayObject("Box", init)
    assert_true(is_valid(h), "Core:createDisplayObject returned invalid handle")
    if is_valid(h) then assert_true(h:getName() == "unitTestBox", "Created name mismatch") end

    -- ✅ Core:countOrphanedDisplayObjects()
    local count = Core:countOrphanedDisplayObjects()
    assert_true(type(count) == "number", "countOrphanedDisplayObjects did not return a number")
    assert_true(count == 1, "Expected 1 orphan after creation; got " .. tostring(count))

    stage:addChild(h)
    count = Core:countOrphanedDisplayObjects(); assert_true(count == 0, "Expected 0 orphans after addChild; got " .. tostring(count))

    stage:removeChild(h)
    count = Core:countOrphanedDisplayObjects(); assert_true(count == 1, "Expected 1 orphan after removeChild; got " .. tostring(count))

    -- ✅ Core:collectGarbage()
    Core:collectGarbage()
    count = Core:countOrphanedDisplayObjects(); assert_true(count == 0, "Expected 0 orphans after collectGarbage; got " .. tostring(count))
end

-- Test 4: Lookup helpers (valid/invalid)
local function test_lookup()
    local s1 = get_stage1(); local s2 = get_stage2()
    if is_valid(s1) and is_valid(s2) then
        -- ✅ Core:getStage() vs Core:getDisplayObject()
        assert_true(s1 ~= s2, "getStage and getDisplayObject('stageTwo') returned same handle")
    end
    -- ✅ Core:getDisplayObject()
    local bad = Core:getDisplayObject("nonexistentStage")
    assert_true(bad == nil or (bad.isValid and not bad:isValid()), "getDisplayObject(nonexistent) should be nil/invalid")
end

-- Test 5: Basic config accessors return sane types
local function test_config_accessors()
    -- ✅ Core:getPixelWidth(), Core:getPixelHeight()
    local w = Core:getPixelWidth(); local h = Core:getPixelHeight()
    assert_true(type(w) == "number" and type(h) == "number" and w > 0 and h > 0, "Pixel size accessors invalid")
    -- ✅ Core:getPixelFormat(), Core:getWindowFlags()
    local fmt = Core:getPixelFormat(); assert_true(type(fmt) == "number" or type(fmt) == "string", "Pixel format invalid")
    local flags = Core:getWindowFlags(); assert_true(type(flags) == "number", "Window flags invalid")
    -- ✅ Core:getWindowWidth(), Core:getWindowHeight()
    local ww = Core:getWindowWidth(); local wh = Core:getWindowHeight()
    assert_true(type(ww) == "number" and type(wh) == "number" and ww > 0 and wh > 0, "Window size accessors invalid")
    -- ✅ Core:getPreserveAspectRatio(), Core:getAllowTextureResize(), Core:getRendererLogicalPresentation()
    local par = Core:getPreserveAspectRatio(); local atr = Core:getAllowTextureResize(); local rlp = Core:getRendererLogicalPresentation()
    assert_true(type(par) == "boolean", "getPreserveAspectRatio did not return boolean")
    assert_true(type(atr) == "boolean", "getAllowTextureResize did not return boolean")
    assert_true(type(rlp) == "number", "getRendererLogicalPresentation did not return integer")
end

-- Gate for exhaustive setter tests (may rebuild device multiple times)
local FULL_CONFIG_TEST = false

-- Test 5b: Config setters (quick), with optional full tests
local function test_config_setters()
    -- Capture current values
    local pw0 = Core:getPixelWidth()
    local ph0 = Core:getPixelHeight()
    local pf0 = Core:getPixelFormat()
    local wf0 = Core:getWindowFlags()

    -- Quick path: set current values (should trigger refresh; must not throw)
    local ok1, e1 = pcall(function() Core:setPixelWidth(pw0) end);       assert_true(ok1, "setPixelWidth threw: " .. tostring(e1))
    local ok2, e2 = pcall(function() Core:setPixelHeight(ph0) end);      assert_true(ok2, "setPixelHeight threw: " .. tostring(e2))
    local ok3, e3 = pcall(function() Core:setPixelFormat(pf0) end);      assert_true(ok3, "setPixelFormat threw: " .. tostring(e3))
    local ok4, e4 = pcall(function() Core:setWindowFlags(wf0) end);      assert_true(ok4, "setWindowFlags threw: " .. tostring(e4))
    local ok5, e5 = pcall(function() Core:setWindowWidth(Core:getWindowWidth()) end);   assert_true(ok5, "setWindowWidth threw: " .. tostring(e5))
    local ok6, e6 = pcall(function() Core:setWindowHeight(Core:getWindowHeight()) end); assert_true(ok6, "setWindowHeight threw: " .. tostring(e6))
    local ok7, e7 = pcall(function() Core:setPreserveAspectRatio(Core:getPreserveAspectRatio()) end); assert_true(ok7, "setPreserveAspectRatio threw: " .. tostring(e7))
    local ok8, e8 = pcall(function() Core:setAllowTextureResize(Core:getAllowTextureResize()) end);   assert_true(ok8, "setAllowTextureResize threw: " .. tostring(e8))
    local ok9, e9 = pcall(function() Core:setRendererLogicalPresentation(Core:getRendererLogicalPresentation()) end); assert_true(ok9, "setRendererLogicalPresentation threw: " .. tostring(e9))

    -- Smoke: engine still functional after refresh
    local root = Core:getRoot(); assert_true(is_valid(root), "Root invalid after config refresh")
    Core:pumpEventsOnce()

    if FULL_CONFIG_TEST then
        -- More exhaustive: nudge pixel sizes and restore
        local function approx(v) return (type(v) == "number" and v) or 0 end
        local pw1 = approx(pw0) + 0.5
        local ph1 = approx(ph0) + 0.5

        local ok5, e5 = pcall(function() Core:setPixelWidth(pw1) end);   assert_true(ok5, "setPixelWidth(+delta) threw: " .. tostring(e5))
        assert_true(math.abs(Core:getPixelWidth() - pw1) < 1e-3, "getPixelWidth mismatch after set")
        local ok6, e6 = pcall(function() Core:setPixelHeight(ph1) end);  assert_true(ok6, "setPixelHeight(+delta) threw: " .. tostring(e6))
        assert_true(math.abs(Core:getPixelHeight() - ph1) < 1e-3, "getPixelHeight mismatch after set")
        -- Window size nudges
        local ww0, wh0 = Core:getWindowWidth(), Core:getWindowHeight()
        local ww1, wh1 = ww0 + 1, wh0 + 1
        local okW1, eW1 = pcall(function() Core:setWindowWidth(ww1) end);   assert_true(okW1, "setWindowWidth(+1) threw: " .. tostring(eW1))
        local okH1, eH1 = pcall(function() Core:setWindowHeight(wh1) end);  assert_true(okH1, "setWindowHeight(+1) threw: " .. tostring(eH1))
        assert_true(math.abs(Core:getWindowWidth() - ww1) < 1e-3, "getWindowWidth mismatch after set")
        assert_true(math.abs(Core:getWindowHeight() - wh1) < 1e-3, "getWindowHeight mismatch after set")

        -- Use SDL_Utils helpers to resolve string forms if available
        local pf_rgba = (SDL_Utils and SDL_Utils.pixelFormatFromString) and SDL_Utils.pixelFormatFromString("SDL_PIXELFORMAT_RGBA8888") or pf0
        local ok7, e7 = pcall(function() Core:setPixelFormat(pf_rgba) end); assert_true(ok7, "setPixelFormat(rgba8888) threw: " .. tostring(e7))

        local wf_resizable = (SDL_Utils and SDL_Utils.windowFlagsFromString) and SDL_Utils.windowFlagsFromString("SDL_WINDOW_RESIZABLE") or wf0
        local ok8, e8 = pcall(function() Core:setWindowFlags(wf_resizable) end); assert_true(ok8, "setWindowFlags(resizable) threw: " .. tostring(e8))

        -- Toggle preserve/allow and restore
        local par0, atr0 = Core:getPreserveAspectRatio(), Core:getAllowTextureResize()
        local okPar, ePar = pcall(function() Core:setPreserveAspectRatio(not par0) end); assert_true(okPar, "setPreserveAspectRatio(toggle) threw: " .. tostring(ePar))
        local okAtr, eAtr = pcall(function() Core:setAllowTextureResize(not atr0) end);   assert_true(okAtr, "setAllowTextureResize(toggle) threw: " .. tostring(eAtr))

        -- Restore originals
        Core:setPixelWidth(pw0)
        Core:setPixelHeight(ph0)
        Core:setWindowWidth(ww0)
        Core:setWindowHeight(wh0)
        Core:setPixelFormat(pf0)
        Core:setWindowFlags(wf0)
        Core:setPreserveAspectRatio(par0)
        Core:setAllowTextureResize(atr0)
        -- Recheck
        assert_true(math.abs(Core:getPixelWidth() - pw0) < 1e-3, "getPixelWidth mismatch after restore")
        assert_true(math.abs(Core:getPixelHeight() - ph0) < 1e-3, "getPixelHeight mismatch after restore")
        assert_true(math.abs(Core:getWindowWidth() - ww0) < 1e-3, "getWindowWidth mismatch after restore")
        assert_true(math.abs(Core:getWindowHeight() - wh0) < 1e-3, "getWindowHeight mismatch after restore")
    end
end

-- Test 6: Window title round-trip
local function test_window_title()
    -- ✅ Core:getWindowTitle(), Core:setWindowTitle()
    local old = Core:getWindowTitle()
    local ok, err = pcall(function() Core:setWindowTitle("CoreUnitTest Title") end)
    assert_true(ok, "setWindowTitle threw: " .. tostring(err))
    local now = Core:getWindowTitle()
    assert_true(type(now) == "string" and #now > 0, "getWindowTitle returned empty")
    -- Restore
    if type(old) == "string" then Core:setWindowTitle(old) end
end

-- Test 7: Focus/hover setters
local function test_focus_hover()
    local obj = Core:getDisplayObject("blueishBox")
    if not is_valid(obj) then return end
    -- ✅ Core:setKeyboardFocusedObject()
    Core:setKeyboardFocusedObject(obj)
    local kf = Core:getKeyboardFocusedObject()
    assert_true(is_valid(kf) and kf:getName() == obj:getName(), "Keyboard focus object mismatch")
    Core:setMouseHoveredObject(obj)
    -- ✅ Core:getMouseHoveredObject()
    local mh = Core:getMouseHoveredObject()
    assert_true(is_valid(mh) and mh:getName() == obj:getName(), "Mouse hovered object mismatch")
end

-- Test 8: Time/event helpers
local function test_time_and_events()
    -- ✅ Core:getElapsedTime(), Core:getDeltaTime()
    local t1 = Core:getElapsedTime(); local dt = Core:getDeltaTime(); assert_true(type(dt) == "number", "getDeltaTime not numeric")
    -- ✅ Core:pushMouseEvent()
    Core:pushMouseEvent({ x = 5, y = 5, type = "down", button = 1 })
    Core:pushMouseEvent({ x = 5, y = 5, type = "up", button = 1 })
    -- ✅ Core:pushKeyboardEvent()
    Core:pushKeyboardEvent({ key = 13, type = "down" })
    -- ✅ Core:pumpEventsOnce()
    Core:pumpEventsOnce()
    -- ✅ Core:getElapsedTime()
    local t2 = Core:getElapsedTime()
    assert_true(type(t1) == "number" and type(t2) == "number", "Elapsed time not numeric")
end

-- Test 9: Asset APIs
local function test_asset_api()
    -- Names configured in examples/test/lua/config.lua
    -- ✅ Core:hasAssetObject(), Core:getAssetObject()
    local hasV = Core:hasAssetObject("VarelaRound16")
    assert_true(type(hasV) == "boolean", "hasAssetObject returned non-boolean")
    if hasV then
        -- ✅ Core:getAssetObject()
        local ah = Core:getAssetObject("VarelaRound16")
        assert_true(ah and ah.isValid and ah:isValid(), "getAssetObject returned invalid handle for VarelaRound16")
    end
end

-- Test 10: Destroy display object (handle/name/table forms)
local function test_destroy_displayobject()
    local name = "lua_temp_obj"
    -- ✅ Core:createDisplayObject()
    local h = Core:createDisplayObject("Box", { name = name, type = "Box", x = 1, y = 1, width = 2, height = 2, color = {255,255,255,255} })
    assert_true(is_valid(h), "Failed to create lua_temp_obj")
    -- ✅ Core:destroyDisplayObject()
    Core:destroyDisplayObject(h)
    -- ✅ Core:getDisplayObject()
    local chk = Core:getDisplayObject(name)
    if chk and chk.isValid and chk:isValid() then
        -- Try by name
        -- ✅ Core:destroyDisplayObject()
        Core:destroyDisplayObject(name)
        -- ✅ Core:getDisplayObject()
        chk = Core:getDisplayObject(name)
    end
    assert_true(chk == nil or (chk.isValid and not chk:isValid()), "destroyDisplayObject did not remove object")
end

-- Test 11: Device rebuild smoke under FULL_CONFIG_TEST
local function test_config_device_smoke()
    if not FULL_CONFIG_TEST then return end
    local stage = get_stage1(); if not is_valid(stage) then return end

    -- Create a small scene with a Label, Frame, and Slider
    local l = Core:createDisplayObject("Label", {
        name = "lua_smoke_label",
        type = "Label",
        text = "smoke",
        font = "VarelaRound16",
        x = 8, y = 8, width = 96, height = 20,
        background = true
    })
    assert_true(is_valid(l), "Smoke: failed to create Label")

    local p = Core:createDisplayObject("Frame", {
        name = "lua_smoke_frame",
        type = "Frame",
        x = 120, y = 8, width = 96, height = 40
    })
    assert_true(is_valid(p), "Smoke: failed to create Frame")

    local s = Core:createDisplayObject("Slider", {
        name = "lua_smoke_slider",
        type = "Slider",
        x = 8, y = 40, width = 128, height = 16,
        value = 50
    })
    assert_true(is_valid(s), "Smoke: failed to create Slider")

    -- Attach to stage and render once
    stage:addChild(l); stage:addChild(p); stage:addChild(s)
    Core:pumpEventsOnce()

    -- Flip a couple of device-affecting settings; expect no exceptions
    local pw0, ph0 = Core:getPixelWidth(), Core:getPixelHeight()
    local pf0, wf0 = Core:getPixelFormat(), Core:getWindowFlags()
    local ok1, e1 = pcall(function() Core:setPixelWidth(pw0 + 0.5) end);  assert_true(ok1, "Smoke: setPixelWidth threw: " .. tostring(e1))
    local ok2, e2 = pcall(function() Core:setPixelHeight(ph0 + 0.5) end); assert_true(ok2, "Smoke: setPixelHeight threw: " .. tostring(e2))

    -- Use SDL_Utils helpers if available for well-known values
    local pf_rgba = (SDL_Utils and SDL_Utils.pixelFormatFromString) and SDL_Utils.pixelFormatFromString("SDL_PIXELFORMAT_RGBA8888") or pf0
    local ok3, e3 = pcall(function() Core:setPixelFormat(pf_rgba) end);    assert_true(ok3, "Smoke: setPixelFormat threw: " .. tostring(e3))

    local wf_resizable = (SDL_Utils and SDL_Utils.windowFlagsFromString) and SDL_Utils.windowFlagsFromString("SDL_WINDOW_RESIZABLE") or wf0
    local ok4, e4 = pcall(function() Core:setWindowFlags(wf_resizable) end); assert_true(ok4, "Smoke: setWindowFlags threw: " .. tostring(e4))

    -- Pump once to allow rebuilds and per-object onWindowResize handling
    Core:pumpEventsOnce()

    -- Ensure objects remain valid
    assert_true(is_valid(l) and is_valid(p) and is_valid(s), "Smoke: objects invalid after device rebuild")

    -- Cleanup
    Core:destroyDisplayObject("lua_smoke_label")
    Core:destroyDisplayObject("lua_smoke_frame")
    Core:destroyDisplayObject("lua_smoke_slider")
end

-- Test 12: Future child management and factory utilities
-- (omitted) Future child management and factory-only utilities are not exposed to Lua yet

-- Runner
local function run_all()
    test_stage_root()
    test_traversing_flag()
    test_creation_orphans()
    test_lookup()
    test_config_accessors()
    test_config_setters()
    test_window_title()
    test_focus_hover()
    test_time_and_events()
    test_asset_api()
    test_destroy_displayobject()
    if FULL_CONFIG_TEST then test_config_device_smoke() end
end

local ok, err = pcall(run_all)
if not ok then push("Core Lua test runner threw: " .. tostring(err)) end

return utils.get_results()
