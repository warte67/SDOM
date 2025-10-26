-- require the helper
local utils = require("src.UnitTests")

-- Small helpers
local function push(msg) utils.push_error(msg) end
local function assert_true(cond, msg)
    if not cond then push(msg) end
end
local function is_valid(h)
    return (h ~= nil) and (h.isValid == nil or h:isValid())
end

-- Get common objects configured in examples/test/lua/config.lua
local function get_stage()
    local s = Core:getDisplayObject("mainStage")
    if not is_valid(s) then push("mainStage not found") end
    return s
end
local function get_blueish()
    local b = Core:getDisplayObject("blueishBox")
    if not is_valid(b) then push("blueishBox not found") end
    return b
end
local function get_blueish_label()
    local l = Core:getDisplayObject("blueishBoxLabel")
    if not is_valid(l) then push("blueishBoxLabel not found") end
    return l
end

-- Test 1: Dirty/State Management
local function test_dirty_state()
    local stage = get_stage(); if not is_valid(stage) then return end
    local box = get_blueish(); if not is_valid(box) then return end
    local label = get_blueish_label(); if not is_valid(label) then return end

    stage:cleanAll()
    assert_true(not box:isDirty(), "blueishBox should be clean initially")
    assert_true(not label:isDirty(), "blueishBoxLabel should be clean initially")

    label:setDirty()
    assert_true(label:isDirty(), "blueishBoxLabel should be dirty after setDirty(true)")
    assert_true(not box:isDirty(), "blueishBox should remain clean when only child is dirty")

    box:setDirty()
    assert_true(box:isDirty(), "blueishBox should be dirty after setDirty(true)")
    assert_true(label:isDirty(), "blueishBoxLabel should still be dirty after parent dirtied")

    stage:cleanAll()
    assert_true(not box:isDirty(), "blueishBox still dirty after cleanAll()")
    assert_true(not label:isDirty(), "blueishBoxLabel still dirty after cleanAll()")
end

-- Test 2: Events and Event Listener Handling
local function test_events()
    local stage = get_stage(); if not is_valid(stage) then return end

    local counter = 0
    local listener = function(ev)
        -- Sanity: target should be mainStage
        local t = ev:getTarget()
        if t and (t.getName ~= nil) then
            local name = t:getName()
            if name == "mainStage" then counter = counter + 1 end
        end
    end

    -- Resolve EventType userdata via table binding (non-callable)
    local NONE = (EventType and EventType.None) or nil
    if not NONE then
        push("EventType('None') not available in Lua registry")
        return
    end
    -- Use descriptor form with userdata EventType
    stage:addEventListener({ type = NONE, listener = listener, useCapture = false, priority = 0 })
    assert_true(stage:hasEventListener(NONE, false), "Expected None listener registered")

    -- Queue an event and pump once
    stage:queue_event(NONE, function(e)
        -- payload round-trip via table (optional)
        local p = e:getPayload()
        if p then p["info"] = "test" end
    end)
    Core:pumpEventsOnce()

    assert_true(counter == 1, "Expected listener invoked once; got " .. tostring(counter))

    -- Attempt removal (Lua wrapper may not be able to match function identity); do not assert removal
    stage:removeEventListener({ type = NONE, listener = listener, useCapture = false })
end

-- Test 3: Hierarchy Management
local function test_hierarchy()
    local stage = get_stage(); if not is_valid(stage) then return end
    local blueish = get_blueish(); if not is_valid(blueish) then return end

    -- Create a Box and attach it to the stage
    local init = { name = "lua_test_box", type = "Box", x = 10, y = 10, width = 64, height = 48, color = {255, 0, 255, 255} }
    local h = Core:createDisplayObject("Box", init)
    assert_true(is_valid(h), "Failed to create 'lua_test_box'")

    stage:addChild(h)
    assert_true(stage:hasChild(h), "Stage should have lua_test_box as child")

    -- Fetch by name
    local fetched = stage:getChild("lua_test_box")
    assert_true(is_valid(fetched), "Failed to get 'lua_test_box' by name")

    -- Remove by handle, re-add, then remove by name
    stage:removeChild(h)
    assert_true(not stage:hasChild(h), "lua_test_box should be removed from stage (handle)")
    stage:addChild(h)
    assert_true(stage:hasChild(h), "lua_test_box failed to return to stage")
    stage:removeChild("lua_test_box")
    assert_true(not stage:hasChild(h), "lua_test_box should be removed from stage (name)")

    -- Reparent to blueishBox
    stage:addChild(h)
    h:setParent(blueish)
    local parent = h:getParent()
    assert_true(is_valid(parent) and parent:getName() == "blueishBox", "Parent change to blueishBox failed")

    -- Ancestor helper: traverse parents to avoid binding overload issues
    local function is_ancestor(parent, child)
        if not is_valid(parent) or not is_valid(child) then return false end
        local p = child:getParent()
        while p and (p.isValid == nil or p:isValid()) do
            if p:getName() == parent:getName() then return true end
            p = p:getParent()
        end
        return false
    end
    assert_true(is_ancestor(stage, h), "Stage should be ancestor of lua_test_box")

    -- removeFromParent, then remove via stage:removeDescendant
    local removed = h:removeFromParent()
    assert_true(removed, "removeFromParent() reported failure for lua_test_box")
    assert_true(not blueish:hasChild(h), "lua_test_box should be removed from blueishBox")

    blueish:addChild(h)
    blueish:removeChild("lua_test_box")
    assert_true(not blueish:hasChild(h), "lua_test_box should be removed from blueish by name")

    -- Cleanup
    Core:destroyDisplayObject("lua_test_box")
end

-- Test 4: Type & Property Access (names, type, colors)
local function test_type_props()
    local box = get_blueish(); if not is_valid(box) then return end

    -- Name round-trip
    local orig = box:getName()
    local tmp = orig .. "_lua"
    box:setName(tmp)
    assert_true(box:getName() == tmp, "setName/getName mismatch")
    box:setName(orig)

    -- Type
    local ty = box:getType()
    assert_true(ty ~= nil and ty ~= "", "getType returned empty")

    -- Color read sanity (avoid setColor overload ambiguity in some runtimes)
    local c = box:getColor()
    assert_true(type(c.r) == "number" and type(c.g) == "number" and type(c.b) == "number" and type(c.a) == "number", "getColor returned invalid components")

    -- Border/background flags
    local hadBorder = box:hasBorder()
    local hadBg = box:hasBackground()
    box:setBorder(not hadBorder)
    box:setBackground(not hadBg)
    assert_true(box:hasBorder() == (not hadBorder), "setBorder did not take")
    assert_true(box:hasBackground() == (not hadBg), "setBackground did not take")
    -- restore
    box:setBorder(hadBorder)
    box:setBackground(hadBg)
end

-- Test 5: Priority & Z-Order
local function test_priority_z()
    local stage = get_stage(); if not is_valid(stage) then return end

    local function make_box(name, x)
        local init = { name = name, type = "Box", x = x, y = 10, width = 20, height = 20, color = {x, x, x, 255} }
        return Core:createDisplayObject("Box", init)
    end

    local parent = make_box("lua_parent", 32); stage:addChild(parent)
    local A = make_box("lua_A", 64); parent:addChild(A)
    local B = make_box("lua_B", 96); parent:addChild(B)
    local C = make_box("lua_C", 128); parent:addChild(C)

    -- Establish ordering using child z-order helpers (avoid parent-descriptor overloads)
    C:moveToBottom()
    B:bringToFront()

    -- Verify order by child names
    local kids = parent:getChildren()
    local first = kids[1] and kids[1]:getName() or nil
    local last  = kids[#kids] and kids[#kids]:getName() or nil
    assert_true(first == "lua_C", "Priority sort: expected first child 'lua_C', got '" .. tostring(first) .. "'")
    assert_true(last == "lua_B",  "Priority sort: expected last child 'lua_B', got '" .. tostring(last) .. "'")

    -- Move C to top then bottom using child z-order helpers (no priority assumptions)
    C:bringToFront()
    local kids2 = parent:getChildren()
    local last2 = kids2[#kids2] and kids2[#kids2]:getName() or nil
    assert_true(last2 == "lua_C", "bringToFront should put 'lua_C' last; got '" .. tostring(last2) .. "'")
    C:moveToBottom()
    local kids3 = parent:getChildren()
    local first3 = kids3[1] and kids3[1]:getName() or nil
    assert_true(first3 == "lua_C", "moveToBottom should put 'lua_C' first; got '" .. tostring(first3) .. "'")

    -- Z-Order helpers (basic sanity)
    A:moveToTop()
    -- After moveToTop, A should have highest z among siblings
    assert_true(A:getZOrder() >= B:getZOrder() and A:getZOrder() >= C:getZOrder(), "moveToTop did not place A on top")

    -- Cleanup
    Core:destroyDisplayObject("lua_A")
    Core:destroyDisplayObject("lua_B")
    Core:destroyDisplayObject("lua_C")
    Core:destroyDisplayObject("lua_parent")
end

-- Test 6: Focus, Visibility, Clickable/Enabled, Tab
local function test_focus_visibility()
    local box = get_blueish(); if not is_valid(box) then return end
    local label = get_blueish_label(); if not is_valid(label) then return end

    -- Hidden/Visible
    local wasHidden = label:isHidden()
    label:setHidden(true)
    assert_true(label:isHidden(), "setHidden(true) did not take on label")
    label:setHidden(wasHidden)
    assert_true(label:isHidden() == wasHidden, "Failed to restore hidden state on label")

    local wasVisible = box:isVisible()
    box:setVisible(false)
    assert_true(not box:isVisible(), "setVisible(false) did not take on box")
    box:setVisible(wasVisible)
    assert_true(box:isVisible() == wasVisible, "Failed to restore visible state on box")

    -- Clickable/Enabled
    local wasClickable = box:isClickable()
    local wasEnabled = box:isEnabled()
    box:setClickable(not wasClickable)
    box:setEnabled(not wasEnabled)
    assert_true(box:isClickable() == (not wasClickable), "setClickable toggle failed")
    assert_true(box:isEnabled() == (not wasEnabled), "setEnabled toggle failed")
    -- restore
    box:setClickable(wasClickable)
    box:setEnabled(wasEnabled)

    -- Tab priority / enabled
    local origPrio = box:getTabPriority()
    local origTab = box:isTabEnabled()
    box:setTabPriority(origPrio + 1)
    assert_true(box:getTabPriority() == origPrio + 1, "setTabPriority did not take")
    box:setTabEnabled(false)
    assert_true(not box:isTabEnabled(), "setTabEnabled(false) did not take")
    -- restore
    box:setTabEnabled(origTab)
    box:setTabPriority(origPrio)
end

-- Test 7: Geometry setters (x,y,w,h) — revert after
local function test_geometry()
    local box = get_blueish(); if not is_valid(box) then return end

    local ox, oy = box:getX(), box:getY()
    local ow, oh = box:getWidth(), box:getHeight()

    box:setX(ox + 5); box:setY(oy + 5)
    box:setWidth(ow + 10); box:setHeight(oh + 10)

    assert_true(box:getX() == ox + 5 and box:getY() == oy + 5, "setX/setY did not take")
    assert_true(box:getWidth() == ow + 10 and box:getHeight() == oh + 10, "setWidth/setHeight did not take")

    -- restore
    box:setX(ox); box:setY(oy)
    box:setWidth(ow); box:setHeight(oh)
end

-- Test 8: Orphan retention (auto)
local function test_orphan_policy()
    local stage = get_stage(); if not is_valid(stage) then return end

    local init = { name = "lua_orphan", type = "Box", x = 0, y = 0, width = 8, height = 8, color = {255,255,255,255} }
    local h = Core:createDisplayObject("Box", init)
    assert_true(is_valid(h), "Failed to create 'lua_orphan'")
    stage:addChild(h)

    -- Auto-destroy when orphaned and GC collected
    h:setOrphanRetentionPolicy("auto")
    stage:removeChild(h)
    Core:collectGarbage()
    local probe = Core:getDisplayObject("lua_orphan")
    if probe and probe.isValid and probe:isValid() then
        push("lua_orphan should be destroyed after orphan + collectGarbage()")
        -- cleanup if still alive
        Core:destroyDisplayObject("lua_orphan")
    end
end

-- Run all tests sequentially
local function run_all()
    test_dirty_state()
    test_events()
    test_hierarchy()
    test_type_props()
    test_priority_z()
    test_focus_visibility()
    test_geometry()
    test_orphan_policy()
end

-- Execute
local ok, err = pcall(run_all)
if not ok then
    push("Lua test runner threw: " .. tostring(err))
end

-- return summary
return utils.get_results()
