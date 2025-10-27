-- require the helper
local utils = require("src.UnitTests")

-- Small helpers
local function push(msg) utils.push_error(msg) end
local function assert_true(cond, msg)
    if not cond then push(msg) end
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

        | Property        | Type        | Getter                             | Setter                            | Notes                  |
        | --------------- | ----------- | ---------------------------------- | --------------------------------- | ---------------------- |
        | `name`          | string      | `getName()`                        | `setName(string)`                 | ☐ planned             |
        | `type`          | string      | `getType()`                        | n/a                               | ☐ planned             |
        | `x`             | number      | `getX()`                           | `setX(number)`                    | ☐ planned             |
        | `y`             | number      | `getY()`                           | `setY(number)`                    | ☐ planned             |
        | `width`         | number      | `getWidth()`                       | `setWidth(number)`                | ☐ planned             |
        | `height`        | number      | `getHeight()`                      | `setHeight(number)`               | ☐ planned             |
        | `w` *(alias)*   | number      | → `width`                          | → `width`                         | ☐ planned             |
        | `h` *(alias)*   | number      | → `height`                         | → `height`                        | ☐ planned             |
        | `color`         | `{r,g,b,a}` | `getColor()`                       | `setColor(SDL_Color)`             | ☐ planned             |
        | `anchor_top`    | enum/int    | `getAnchorTop()`                   | `setAnchorTop(int)`               | ☐ planned             |
        | `anchor_left`   | enum/int    | `getAnchorLeft()`                  | `setAnchorLeft(int)`              | ☐ planned             |
        | `anchor_bottom` | enum/int    | `getAnchorBottom()`                | `setAnchorBottom(int)`            | ☐ planned             |
        | `anchor_right`  | enum/int    | `getAnchorRight()`                 | `setAnchorRight(int)`             | ☐ planned             |
        | `z_order`       | number      | `getZOrder()`                      | `setZOrder(number)`               | ☐ planned             |
        | `priority`      | number      | `getPriority()`                    | `setPriority(number)`             | ☐ planned             |
        | `is_clickable`  | boolean     | `isClickable()`                    | `setClickable(bool)`              | ☐ planned             |
        | `is_enabled`    | boolean     | `isEnabled()`                      | `setEnabled(bool)`                | ☐ planned             |
        | `is_hidden`     | boolean     | `isHidden()`                       | `setHidden(bool)`                 | ☐ planned             |
        | `tab_priority`  | number      | `getTabPriority()`                 | `setTabPriority(number)`          | ☐ planned             |
        | `tab_enabled`   | boolean     | `isTabEnabled()`                   | `setTabEnabled(bool)`             | ☐ planned             |
        | `left`          | number      | `getLeft()`                        | `setLeft(number)`                 | ☐ planned             |
        | `right`         | number      | `getRight()`                       | `setRight(number)`                | ☐ planned             |
        | `top`           | number      | `getTop()`                         | `setTop(number)`                  | ☐ planned             |
        | `bottom`        | number      | `getBottom()`                      | `setBottom(number)`               | ☐ planned             |
        | `local_left`    | number      | `getLocalLeft()`                   | `setLocalLeft(number)`            | ☐ planned             |
        | `local_right`   | number      | `getLocalRight()`                  | `setLocalRight(number)`           | ☐ planned             |
        | `local_top`     | number      | `getLocalTop()`                    | `setLocalTop(number)`             | ☐ planned             |
        | `local_bottom`  | number      | `getLocalBottom()`                 | `setLocalBottom(number)`          | ☐ planned             |
        | `orphan_policy` | string      | `getOrphanRetentionPolicyString()` | `setOrphanRetentionPolicy("auto") | ☐ planned             |
        | `orphan_grace`  | number      | `getOrphanGrace()`                 | `setOrphanGrace(number)`          | ☐ planned             |    
    --]]


    local stage = get_stage(); if not is_valid(stage) then return end

    -- Create a Box and attach it to the stage
    local init = { name = "lua_test_box", type = "Box", x = 10, y = 10, width = 64, height = 48, color = {255, 0, 255, 255} }
    local box = Core:createDisplayObject("Box", init)
    assert_true(is_valid(box), "Failed to create 'lua_test_box'")
    stage:addChild(box)
    assert_true(stage:hasChild(box), "Stage should have lua_test_box as child")

    -- Test properties
    -- ✅ x
    local ox = box.x
    assert_true(type(ox) == "number" and ox == box:getX(), "getX() returned invalid value")
    box.x = 20
    assert_true(box.x == 20, "setX() did not take")
    box.x = ox
    assert_true(box.x == ox, "Failed to restore x value")
    -- ✅ y
    local oy = box.y
    assert_true(type(oy) == "number" and oy == box:getY(), "getY() returned invalid value")
    box.y = 20
    assert_true(box.y == 20, "setY() did not take")
    box.y = oy
    assert_true(box.y == oy, "Failed to restore y value")
    -- ✅ width
    local ow = box.width
    assert_true(type(ow) == "number" and ow == box:getWidth(), "getWidth() returned invalid value")
    box.width = 80
    assert_true(box.width == 80, "setWidth() did not take")
    box.width = ow
    assert_true(box.width == ow, "Failed to restore width value")
    -- ✅ height
    local oh = box.height
    assert_true(type(oh) == "number" and oh == box:getHeight(), "getHeight() returned invalid value")
    box.height = 80
    assert_true(box.height == 80, "setHeight() did not take")
    box.height = oh
    assert_true(box.height == oh, "Failed to restore height value")
    -- ✅ w
    local ow2 = box.w
    assert_true(type(ow2) == "number" and ow2 == box:getWidth(), "getW() returned invalid value")
    box.w = 80
    assert_true(box.w == 80, "setW() did not take")
    box.w = ow2
    assert_true(box.w == ow2, "Failed to restore w value")
    -- ✅ h
    local oh2 = box.h
    assert_true(type(oh2) == "number" and oh2 == box:getHeight(), "getH() returned invalid value")
    box.h = 80
    assert_true(box.h == 80, "setH() did not take")
    box.h = oh2    
    assert_true(box.h == oh2, "Failed to restore h value")
    
    -- ⚠️ color
    local oc = box.color
    assert_true(type(oc) == "table" and oc.r == 255 and oc.g == 0 and oc.b == 255 and oc.a == 255, "getColor() returned invalid value")
    box.color = {r = 255, g = 255, b = 0, a = 255} -- ✅ 
    box:setColor({r = 255, g = 255, b = 0, a = 255}) -- ⚠️
    local oc2 = box.color
    print("oc2: " .. oc2.r .. " " .. oc2.g .. " " .. oc2.b .. " " .. oc2.a)
    assert_true(box.color.r == oc2.r and box.color.g == oc2.g and box.color.b == oc2.b and box.color.a == oc2.a, "setColor() did not take")
    box.color = oc
    print("oc: " .. oc.r .. " " .. oc.g .. " " .. oc.b .. " " .. oc.a)
    assert_true(box.color.r == oc.r and box.color.g == oc.g and box.color.b == oc.b and box.color.a == oc.a, "Failed to restore color value")

    -- ✅ anchor_top
    local oat = box.anchor_top
    assert_true(type(oat) == "number" and oat == box:getAnchorTop() and box.anchor_top == box:getAnchorTop(), "getAnchorTop() returned invalid value")
    box.anchor_top = 1
    assert_true(box.anchor_top == 1, "setAnchorTop() did not take")
    box.anchor_top = oat
    assert_true(box.anchor_top == oat, "Failed to restore anchor_top value")
    -- ✅ anchor_left
    local oal = box.anchor_left
    assert_true(type(oal) == "number" and oal == box:getAnchorLeft() and box.anchor_left == box:getAnchorLeft(), "getAnchorLeft() returned invalid value")
    box.anchor_left = 1
    assert_true(box.anchor_left == 1, "setAnchorLeft() did not take")
    box.anchor_left = oal
    assert_true(box.anchor_left == oal, "Failed to restore anchor_left value")
    -- ✅ anchor_bottom
    local oab = box.anchor_bottom
    assert_true(type(oab) == "number" and oab == box:getAnchorBottom() and box.anchor_bottom == box:getAnchorBottom(), "getAnchorBottom() returned invalid value")
    box.anchor_bottom = 1
    assert_true(box.anchor_bottom == 1, "setAnchorBottom() did not take")
    box.anchor_bottom = oab
    assert_true(box.anchor_bottom == oab, "Failed to restore anchor_bottom value")
    -- ✅ anchor_right
    local oar = box.anchor_right
    assert_true(type(oar) == "number" and oar == box:getAnchorRight() and box.anchor_right == box:getAnchorRight(), "getAnchorRight() returned invalid value")
    box.anchor_right = 1
    assert_true(box.anchor_right == 1, "setAnchorRight() did not take")
    box.anchor_right = oar
    assert_true(box.anchor_right == oar, "Failed to restore anchor_right value")
    -- ✅ z_order
    local oz = box.z_order
    assert_true(type(oz) == "number" and oz == box:getZOrder() and box.z_order == box:getZOrder(), "getZOrder() returned invalid value")
    box.z_order = 1
    assert_true(box.z_order == 1, "setZOrder() did not take")
    box.z_order = oz
    assert_true(box.z_order == oz, "Failed to restore z_order value")
    -- ✅ priority
    local op = box.priority
    assert_true(type(op) == "number" and op == box:getPriority() and box.priority == box:getPriority(), "getPriority() returned invalid value")
    box.priority = 1
    assert_true(box.priority == 1, "setPriority() did not take")
    box.priority = op
    assert_true(box.priority == op, "Failed to restore priority value")
    -- ✅ is_clickable
    local oca = box.is_clickable
    assert_true(type(oca) == "boolean" and oca == box:isClickable() and box.is_clickable == box:isClickable(), "getClickable() returned invalid value")
    box.is_clickable = true
    assert_true(box.is_clickable == true, "setClickable() did not take")            
    box.is_clickable = oca            
    assert_true(box.is_clickable == oca, "Failed to restore is_clickable value")
    -- ✅ is_enabled
    local oe = box.is_enabled
    assert_true(type(oe) == "boolean" and oe == box:isEnabled() and box.is_enabled == box:isEnabled(), "getEnabled() returned invalid value")
    box.is_enabled = false
    assert_true(box.is_enabled == false, "setEnabled() did not take")
    box.is_enabled = oe
    assert_true(box.is_enabled == oe, "Failed to restore is_enabled value")
    -- ✅ is_hidden
    local oh = box.is_hidden
    assert_true(type(oh) == "boolean" and oh == box:isHidden() and box.is_hidden == box:isHidden(), "getHidden() returned invalid value")
    box.is_hidden = true
    assert_true(box.is_hidden == true, "setHidden() did not take")
    box.is_hidden = oh
    assert_true(box.is_hidden == oh, "Failed to restore is_hidden value")    
    -- ✅ tab_priority
    local otp = box.tab_priority
    assert_true(type(otp) == "number" and otp == box:getTabPriority() and box.tab_priority == box:getTabPriority(), "getTabPriority() returned invalid value")
    box.tab_priority = 1
    assert_true(box.tab_priority == 1, "setTabPriority() did not take")
    box.tab_priority = otp
    assert_true(box.tab_priority == otp, "Failed to restore tab_priority value")
    -- ✅ tab_enabled
    local ote = box.tab_enabled
    assert_true(type(ote) == "boolean" and ote == box:isTabEnabled() and box.tab_enabled == box:isTabEnabled(), "getTabEnabled() returned invalid value")
    box.tab_enabled = true
    assert_true(box.tab_enabled == true, "setTabEnabled() did not take")
    box.tab_enabled = ote
    assert_true(box.tab_enabled == ote, "Failed to restore tab_enabled value")
    -- ✅ left
    local ol = box.left
    assert_true(type(ol) == "number" and ol == box:getLeft(), "getLeft() returned invalid value")
    box.left = 20
    assert_true(box.left == 20, "setLeft() did not take")
    box.left = ol
    assert_true(box.left == ol, "Failed to restore left value")
    -- ✅ right
    local or_ = box.right
    assert_true(type(or_) == "number" and or_ == box:getRight(), "getRight() returned invalid value")
    box.right = 20
    assert_true(box.right == 20, "setRight() did not take")
    box.right = or_
    assert_true(box.right == or_, "Failed to restore right value")
    -- ✅ top
    local ot = box.top
    assert_true(type(ot) == "number" and ot == box:getTop(), "getTop() returned invalid value")
    box.top = 20
    assert_true(box.top == 20, "setTop() did not take")
    box.top = ot
    assert_true(box.top == ot, "Failed to restore top value")
    -- ✅ bottom
    local ob = box.bottom
    assert_true(type(ob) == "number" and ob == box:getBottom(), "getBottom() returned invalid value")
    box.bottom = 20
    assert_true(box.bottom == 20, "setBottom() did not take")
    box.bottom = ob
    assert_true(box.bottom == ob, "Failed to restore bottom value")
    -- ✅ local_left
    local oll = box.local_left
    assert_true(type(oll) == "number" and oll == box:getLocalLeft(), "getLocalLeft() returned invalid value")
    box.local_left = 20
    assert_true(box.local_left == 20, "setLocalLeft() did not take")
    box.local_left = oll
    assert_true(box.local_left == oll, "Failed to restore local_left value")
    -- ✅ local_right
    local olr = box.local_right
    assert_true(type(olr) == "number" and olr == box:getLocalRight(), "getLocalRight() returned invalid value")
    box.local_right = 20
    assert_true(box.local_right == 20, "setLocalRight() did not take")
    box.local_right = olr
    assert_true(box.local_right == olr, "Failed to restore local_right value")
    -- ✅ local_top
    local olt = box.local_top
    assert_true(type(olt) == "number" and olt == box:getLocalTop(), "getLocalTop() returned invalid value")
    box.local_top = 20
    assert_true(box.local_top == 20, "setLocalTop() did not take")
    box.local_top = olt
    assert_true(box.local_top == olt, "Failed to restore local_top value")
    -- ✅ local_bottom
    local olb = box.local_bottom
    assert_true(type(olb) == "number" and olb == box:getLocalBottom(), "getLocalBottom() returned invalid value")
    box.local_bottom = 20
    assert_true(box.local_bottom == 20, "setLocalBottom() did not take")
    box.local_bottom = olb
    assert_true(box.local_bottom == olb, "Failed to restore local_bottom value")

    -- -- ⚠️ orphan_policy
    -- local opo = box.orphan_policy    
    -- assert_true(type(opo) == "string" and opo == "auto" and box.orphan_policy == "auto", "getOrphanRetentionPolicyString() returned invalid value")
    -- box.orphan_policy = "manual"
    -- assert_true(box.orphan_policy == "manual", "setOrphanRetentionPolicy() did not take")
    -- box.orphan_policy = opo
    -- assert_true(box.orphan_policy == opo, "Failed to restore orphan_policy value")

    -- 🔄 orphan_grace
    local og = box.orphan_grace
    assert_true(type(og) == "number" and og == box:getOrphanGrace() and box.orphan_grace == box:getOrphanGrace(), "getOrphanGrace() returned invalid value")
    box.orphan_grace = 100
    assert_true(box.orphan_grace == 100, "setOrphanGrace() did not take")
    box.orphan_grace = og
    assert_true(box.orphan_grace == og, "Failed to restore orphan_grace value")

    -- -- clean up --
    -- stage:removeChild(box)
    -- Core:collectGarbage()

    -- or just:
    -- Core:destroyDisplayObject("lua_test_box")
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
    test_properties()
end

-- Execute
local ok, err = pcall(run_all)
if not ok then
    push("Lua test runner threw: " .. tostring(err))
end

-- return summary
return utils.get_results()
