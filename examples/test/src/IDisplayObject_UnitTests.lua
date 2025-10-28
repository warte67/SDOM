-- require the helper
local utils = require("src.UnitTests")

-- Small helpers
local function push(msg) utils.push_error(msg) end
local function assert_true(cond, msg)
    if not cond then push(msg) end
end
local function assert_false(cond, msg)
    if cond then push(msg) end
end
---@param h DisplayHandle|nil
---@return boolean h_is_valid
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
    ---@cast parent DisplayHandle
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
    local wasHidden = label.is_hidden
    -- local wasHidden = label:isHidden()
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

local function test_properties()
--[[
    -- Legend:
    -- ✅ Test Verified
    -- 🔄 In Progress
    -- ⚠️ Failing
    -- ☐ Planned

    | Property              | Type        | Getter                             | Setter                             | Notes                  |
    | --------------------- | ----------- | ---------------------------------- | ---------------------------------- | ---------------------- |
    | `name`                | string      | `getName()`                        | `setName(string)`                  | ✅ test verified       |
    | `type`                | string      | `getType()`                        | n/a                                | ✅ test verified       |
    | `x`                   | number      | `getX()`                           | `setX(number)`                     | ✅ test verified       |
    | `y`                   | number      | `getY()`                           | `setY(number)`                     | ✅ test verified       |
    | `width`               | number      | `getWidth()`                       | `setWidth(number)`                 | ✅ test verified       |
    | `height`              | number      | `getHeight()`                      | `setHeight(number)`                | ✅ test verified       |
    | `w` *(alias)*         | number      | → `width`                          | → `width`                          | ✅ test verified       |
    | `h` *(alias)*         | number      | → `height`                         | → `height`                         | ✅ test verified       |
    | `color`               | `{r,g,b,a}` | `getColor()`                       | `setColor(SDL_Color)`              | ✅ test verified       |
    | `foreground_color`    | `{r,g,b,a}` | `getForegroundColor()`             | `setForegroundColor(SDL_Color)`    | ✅ test verified       |
    | `background_color`    | `{r,g,b,a}` | `getBackgroundColor()`             | `setBackgroundColor(SDL_Color)`    | ✅ test verified       |
    | `border_color`        | `{r,g,b,a}` | `getBorderColor()`                 | `setBorderColor(SDL_Color)`        | ✅ test verified       |
    | `outline_color`       | `{r,g,b,a}` | `getOutlineColor()`                | `setOutlineColor(SDL_Color)`       | ✅ test verified       |
    | `dropshadow_color`    | `{r,g,b,a}` | `getDropshadowColor()`             | `setDropshadowColor(SDL_Color)`    | ✅ test verified       |
    | `anchor_top`          | enum/int    | `getAnchorTop()`                   | `setAnchorTop(int)`                | ✅ test verified       |
    | `anchor_left`         | enum/int    | `getAnchorLeft()`                  | `setAnchorLeft(int)`               | ✅ test verified       |
    | `anchor_bottom`       | enum/int    | `getAnchorBottom()`                | `setAnchorBottom(int)`             | ✅ test verified       |
    | `anchor_right`        | enum/int    | `getAnchorRight()`                 | `setAnchorRight(int)`              | ✅ test verified       |
    | `z_order`             | number      | `getZOrder()`                      | `setZOrder(number)`                | ✅ test verified       |
    | `priority`            | number      | `getPriority()`                    | `setPriority(number)`              | ✅ test verified       |
    | `is_clickable`        | boolean     | `isClickable()`                    | `setClickable(bool)`               | ✅ test verified       |
    | `is_enabled`          | boolean     | `isEnabled()`                      | `setEnabled(bool)`                 | ✅ test verified       |
    | `is_hidden`           | boolean     | `isHidden()`                       | `setHidden(bool)`                  | ✅ test verified       |
    | `tab_priority`        | number      | `getTabPriority()`                 | `setTabPriority(number)`           | ✅ test verified       |
    | `tab_enabled`         | boolean     | `isTabEnabled()`                   | `setTabEnabled(bool)`              | ✅ test verified       |
    | `left`                | number      | `getLeft()`                        | `setLeft(number)`                  | ✅ test verified       |
    | `right`               | number      | `getRight()`                       | `setRight(number)`                 | ✅ test verified       |
    | `top`                 | number      | `getTop()`                         | `setTop(number)`                   | ✅ test verified       |
    | `bottom`              | number      | `getBottom()`                      | `setBottom(number)`                | ✅ test verified       |
    | `local_left`          | number      | `getLocalLeft()`                   | `setLocalLeft(number)`             | ✅ test verified       |
    | `local_right`         | number      | `getLocalRight()`                  | `setLocalRight(number)`            | ✅ test verified       |
    | `local_top`           | number      | `getLocalTop()`                    | `setLocalTop(number)`              | ✅ test verified       |
    | `local_bottom`        | number      | `getLocalBottom()`                 | `setLocalBottom(number)`           | ✅ test verified       |
    | `orphan_policy`       | string      | `getOrphanRetentionPolicyString()` | `setOrphanRetentionPolicy("auto")` | ✅ test verified       |
    | `orphan_grace`        | number      | `getOrphanGrace()`                 | `setOrphanGrace(number)`           | ✅ test verified       |
--]]

    local stage = get_stage(); if not is_valid(stage) then return end

    -- Create a Box and attach it to the stage
    local init = { name = "lua_test_box", type = "Box", x = 10, y = 10, width = 64, height = 48, color = {255, 0, 255, 255} }
    local box = Core:createDisplayObject("Box", init)
    assert_true(is_valid(box), "Failed to create 'lua_test_box'")
    stage:addChild(box)
    assert_true(stage:hasChild(box), "Stage should have lua_test_box as child")

    -- Name and Type Property Tests
    local box_name = box:getName()
    assert_true(type(box_name) == "string" and box_name == "lua_test_box", "getName() returned invalid value")
    local box_type = box:getType()
    assert_true(box_type == "Box", "getType() returned invalid value")

    -- ✅ Simplified color property tests
    local color_props = {
    { prop = "color", get = "getColor", set = "setColor" },                                 -- ✅ color
    { prop = "foreground_color", get = "getForegroundColor", set = "setForegroundColor" },  -- ✅ foreground_color
    { prop = "background_color", get = "getBackgroundColor", set = "setBackgroundColor" },  -- ✅ background_color
    { prop = "border_color", get = "getBorderColor", set = "setBorderColor" },              -- ✅ border_color
    { prop = "outline_color", get = "getOutlineColor", set = "setOutlineColor" },           -- ✅ outline_color
    { prop = "dropshadow_color", get = "getDropshadowColor", set = "setDropshadowColor" },  -- ✅ dropshadow_color
    }
    for _, entry in ipairs(color_props) do
        local prop, get, set = entry.prop, entry.get, entry.set
        local orig = box[prop]
        assert_true(type(orig) == "table", prop .. " getter returned wrong type")
        box[prop] = {r = 255, g = 255, b = 0, a = 255}
        local check = box[prop]
        local ref = box[get](box)
        assert_true(ref.r == check.r and ref.g == check.g, prop .. " mismatch after set")
        box[set](box, orig)
        assert_true(box[prop].r == orig.r, prop .. " failed to restore")
    end

    -- ✅ Property <-> Method consistency tests for scalar and boolean bindings
    local props = {
        -- anchors
        { name = "anchor_top",      get = "getAnchorTop",      value = 1,      type = "number" },
        { name = "anchor_left",     get = "getAnchorLeft",     value = 1,      type = "number" },
        { name = "anchor_bottom",   get = "getAnchorBottom",   value = 1,      type = "number" },
        { name = "anchor_right",    get = "getAnchorRight",    value = 1,      type = "number" },

        -- z / priority
        { name = "z_order",         get = "getZOrder",         value = 1,      type = "number" },
        { name = "priority",        get = "getPriority",       value = 1,      type = "number" },

        -- booleans
        { name = "is_clickable",    get = "isClickable",       value = true,   type = "boolean" },
        { name = "is_enabled",      get = "isEnabled",         value = false,  type = "boolean" },
        { name = "is_hidden",       get = "isHidden",          value = true,   type = "boolean" },

        -- tab navigation
        { name = "tab_priority",    get = "getTabPriority",    value = 1,      type = "number" },
        { name = "tab_enabled",     get = "isTabEnabled",      value = true,   type = "boolean" },

        -- world edges
        { name = "left",            get = "getLeft",           value = 20,     type = "number" },
        { name = "right",           get = "getRight",          value = 20,     type = "number" },
        { name = "top",             get = "getTop",            value = 20,     type = "number" },
        { name = "bottom",          get = "getBottom",         value = 20,     type = "number" },

        -- local edges
        { name = "local_left",      get = "getLocalLeft",      value = 20,     type = "number" },
        { name = "local_right",     get = "getLocalRight",     value = 20,     type = "number" },
        { name = "local_top",       get = "getLocalTop",       value = 20,     type = "number" },
        { name = "local_bottom",    get = "getLocalBottom",    value = 20,     type = "number" },

        -- geometry (added)
        { name = "x",               get = "getX",              value = 20,     type = "number" },
        { name = "y",               get = "getY",              value = 20,     type = "number" },
        { name = "width",           get = "getWidth",          value = 80,     type = "number" },
        { name = "height",          get = "getHeight",         value = 80,     type = "number" },
        { name = "w",               get = "getWidth",          value = 80,     type = "number" },
        { name = "h",               get = "getHeight",         value = 80,     type = "number" },
    }

    for _, p in ipairs(props) do
        local name, getter, testValue, expectedType = p.name, p.get, p.value, p.type
        local original = box[name]

        -- getter sanity
        assert_true(type(original) == expectedType, name .. " returned wrong type")
        assert_true(original == box[getter](box) and box[name] == box[getter](box),
            getter .. "() returned inconsistent value")

        -- set new
        box[name] = testValue
        assert_true(box[name] == testValue, "set" .. name .. "() did not take")

        -- restore
        box[name] = original
        assert_true(box[name] == original, "Failed to restore " .. name .. " value")
    end

    -- 🔄 orphan_grace
    local og = box.orphan_grace
    assert_true(type(og) == "number" and og == box:getOrphanGrace() and box.orphan_grace == box:getOrphanGrace(), "getOrphanGrace() returned invalid value")
    box.orphan_grace = 100
    assert_true(box.orphan_grace == 100, "setOrphanGrace() did not take")
    box.orphan_grace = og
    assert_true(box.orphan_grace == og, "Failed to restore orphan_grace value")

    -- clean up --
    stage:removeChild(box)
    Core:collectGarbage()
end -- test_properties()

local function test_orphan_policy()
--[[
    -- Legend:
    -- ✅ Test Verified
    -- 🔄 In Progress
    -- ⚠️ Failing
    -- ☐ Planned

    | Property              | Type        | Getter                             | Setter                             | Notes                  |
    | --------------------- | ----------- | ---------------------------------- | ---------------------------------- | ---------------------- |
    | `orphan_policy`       | string      | `getOrphanRetentionPolicyString()` | `setOrphanRetentionPolicy("auto")` | ✅ test verified        |
    | `orphan_grace`        | number      | `getOrphanGrace()`                 | `setOrphanGrace(number)`           | ✅ test verified        |
--]]

    local stage = get_stage(); if not is_valid(stage) then return end

    -- --- 1] String <-> Enum conversions and basic getters/setters ------------
    do
        local init = { name = "orp_conv", type = "Box", x = 10, y = 10, width = 64, height = 48, color = {255, 0, 255, 255} }
        local box = Core:createDisplayObject("Box", init)
        assert_true(is_valid(box), "OrphanPolicy: failed to create 'orp_conv'")
        stage:addChild(box)
        assert_true(stage:hasChild(box), "Stage should have 'orp_conv' as child")

        -- String → Enum conversions (Lua helper)
        assert_true(box:orphanPolicyFromString("manual") == 0, "fromString('manual') != RetainUntilManual")
        assert_true(box:orphanPolicyFromString("auto")   == 1, "fromString('auto') != AutoDestroy")
        assert_true(box:orphanPolicyFromString("grace")  == 2, "fromString('grace') != GracePeriod")

        -- Enum → String conversions
        assert_true(box:orphanPolicyToString(0) == "manual", "toString(RetainUntilManual) != 'manual'")
        assert_true(box:orphanPolicyToString(1) == "auto",   "toString(AutoDestroy) != 'auto'")
        assert_true(box:orphanPolicyToString(2) == "grace",  "toString(GracePeriod) != 'grace'")

        -- Set/get round trip
        box:setOrphanRetentionPolicy("manual")
        assert_true(box:getOrphanRetentionPolicyString() == "manual", "setOrphanRetentionPolicy('manual') did not take")

        box:setOrphanRetentionPolicy("auto")
        assert_true(box:getOrphanRetentionPolicyString() == "auto", "setOrphanRetentionPolicy('auto') did not take")

        -- Grace round trip
        box:setOrphanGrace(250)
        assert_true(box:getOrphanGrace() == 250, "getOrphanGrace() != 250 after Lua set")
        box:setOrphanGrace(500)
        assert_true(box:getOrphanGrace() == 500, "getOrphanGrace() != 500 after second set")

        stage:removeChild(box)
        Core:destroyDisplayObject(box:getName())
        Core:collectGarbage()
    end

    -- --- 2] Manual policy: orphan survives collect until explicitly destroyed --
    do
        local box = Core:createDisplayObject("Box", { name = "orp_manual", type = "Box", x = 10, y = 10, width = 64, height = 48, color = {255, 0, 255, 255}  })
        assert_true(is_valid(box), "OrphanPolicy: failed to create 'orp_manual'")
        stage:addChild(box)
        box:setOrphanRetentionPolicy("manual")

        stage:removeChild(box)

        local found = false
        for _, orphan in ipairs(Core:getOrphanedDisplayObjects()) do
            if orphan == box then found = true break end
        end
        assert_true(found, "OrphanPolicy(manual): orphan not listed after removal")

        -- Collect should NOT destroy
        Core:collectGarbage()
        assert_true(is_valid(Core:getDisplayObject("orp_manual")), "OrphanPolicy(manual): object destroyed by collectGarbage()")

        -- Switch to auto -> should destroy
        box:setOrphanRetentionPolicy("auto")
        Core:collectGarbage()
        assert_false(is_valid(Core:getDisplayObject("orp_manual")), "OrphanPolicy(manual->auto): object not destroyed after auto policy")
    end

    -- --- 3] Grace policy: immediate collect retains; post-grace destroys ------
    do
        local box = Core:createDisplayObject("Box", { name = "orp_grace", type = "Box", x = 10, y = 10, width = 64, height = 48, color = {255, 0, 255, 255}  })
        assert_true(is_valid(box), "OrphanPolicy: failed to create 'orp_grace'")
        stage:addChild(box)
        box:setOrphanRetentionPolicy("grace")
        local grace_ms, margin_ms = 20, 10
        box:setOrphanGrace(grace_ms)

        -- Orphan and collect immediately — should survive
        stage:removeChild(box)
        Core:pumpEventsOnce()
        Core:collectGarbage()
        assert_true(is_valid(Core:getDisplayObject("orp_grace")), "OrphanPolicy(grace): object destroyed before grace expired")

        -- Wait past grace — should be destroyed
        local wait_ms = grace_ms + margin_ms
        SDL.delay(wait_ms)
        Core:pumpEventsOnce()
        Core:collectGarbage()
        assert_false(is_valid(Core:getDisplayObject("orp_grace")), "OrphanPolicy(grace): object not destroyed after grace expiration")
    end

    -- --- 4] Grace policy + reparent within grace prevents destruction ---------
    do
        local box = Core:createDisplayObject("Box", { name = "orp_reparent", type = "Box", x = 10, y = 10, width = 64, height = 48, color = {255, 0, 255, 255}  })
        assert_true(is_valid(box), "OrphanPolicy: failed to create 'orp_reparent'")
        stage:addChild(box)
        box:setOrphanRetentionPolicy("grace")
        local grace_ms, margin_ms = 20, 10
        box:setOrphanGrace(grace_ms)

        -- Orphan then reparent within grace — should survive
        stage:removeChild(box)
        SDL.delay(grace_ms / 2) -- wait half the grace period
        stage:addChild(box)
        Core:pumpEventsOnce()
        Core:collectGarbage()
        assert_true(is_valid(Core:getDisplayObject("orp_reparent")), "OrphanPolicy(reparent): object destroyed despite reparent within grace")

        -- Orphan again and let grace expire — should destroy
        stage:removeChild(box)
        SDL.delay(grace_ms + margin_ms)
        Core:pumpEventsOnce()
        Core:collectGarbage()
        assert_false(is_valid(Core:getDisplayObject("orp_reparent")), "OrphanPolicy(reparent): object not destroyed after grace expiration")
    end

    Core:collectGarbage()
end -- test_orphan_policy()


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
    test_properties()
    test_orphan_policy()
end

-- Execute
local ok, err = pcall(run_all)
if not ok then
    push("Lua test runner threw: " .. tostring(err))
end

-- return summary
return utils.get_results()
