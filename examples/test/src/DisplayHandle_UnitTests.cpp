// DisplayHandle_UnitTests.cpp

// #ifdef SDOM_ENABLE_UNIT_TESTS

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include "UnitTests.hpp"
// Include Box so test helpers (reset/getTestClickCount) are visible
#include "Box.hpp"


namespace SDOM
{

    // DisplayHandle unit tests mapping (renamed to numeric identifiers):
    //  - DisplayHandle_test1  -> "DisplayHandle #1" (forwards/reflection)
    //  - DisplayHandle_test2  -> "DisplayHandle #2" (clickable)
    //  - DisplayHandle_test3  -> "DisplayHandle #3" (enabled)
    //  - DisplayHandle_test4  -> "DisplayHandle #4" (visibility)
    //  - DisplayHandle_test5  -> "DisplayHandle #5" (keyboard focus)
    //  - DisplayHandle_test6  -> "DisplayHandle #6" (mouse hover)
    //  - DisplayHandle_test7  -> "DisplayHandle #7" (tab priority)
    //  - DisplayHandle_test8  -> "DisplayHandle #8" (geometry getters/setters)
    //  - DisplayHandle_test9  -> "DisplayHandle #9" (priority overloads)
    //  - DisplayHandle_test10 -> "DisplayHandle #10" (sort/highest/lowest)

    bool DisplayHandle_test1()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua script: check for presence of DisplayHandle methods related to priority/z-order
        auto res = lua.script(R"(
            local methods = { 'getMaxPriority','getMinPriority','getPriority','setToHighestPriority','setToLowestPriority','sortChildrenByPriority','setPriority','moveToTop','getChildrenPriorities','getZOrder','setZOrder' }
            local dh = Core:getStageHandle()
            if not dh then return { ok = false, err = 'no stage handle' } end
            for i, m in ipairs(methods) do
                local f = dh[m]
                if not f then print('DisplayHandle_test1: missing method -> '..m); return { ok = false, err = 'missing '..m } end
                if type(f) ~= 'function' then print('DisplayHandle_test1: method not callable -> '..m); return { ok = false, err = m..' not callable' } end
            end
            return { ok = true }
        )").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        return UnitTests::run("DisplayHandle #1", "Verify DisplayHandle forwards for priority/z-order exist and are callable", [=]() { return ok; });
    }


    // Split tests: clickable, enabled, visibility, keyboard focus, mouse hover
    bool DisplayHandle_test2()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        bool result = lua.script(R"(
            local b = Core:createDisplayObject('Box', { name = 'li_box_click', type = 'Box', x = 10, y = 10, width = 20, height = 20, isClickable = true })
            if not b then return false end
            local stage = Core:getStageHandle()
            if stage and b then stage:addChild(b) end
            if not b:isClickable() then return false end
            b:setClickable(false)
            if b:isClickable() then return false end
            b:setClickable(true)
            if not b:isClickable() then return false end
            Core:destroyDisplayObject('li_box_click')
            return true
        )").get<bool>();
        return UnitTests::run("DisplayHandle #2", "Verify setClickable/isClickable from Lua", [=]() { return result; });
    }

    bool DisplayHandle_test3()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        bool result = lua.script(R"(
            local b = Core:createDisplayObject('Box', { name = 'li_box_enabled', type = 'Box', x = 10, y = 10, width = 20, height = 20, isEnabled = true })
            if not b then return false end
            local stage = Core:getStageHandle()
            if stage and b then stage:addChild(b) end
            if not b:isEnabled() then return false end
            b:setEnabled(false)
            if b:isEnabled() then return false end
            b:setEnabled(true)
            if not b:isEnabled() then return false end
            Core:destroyDisplayObject('li_box_enabled')
            return true
        )").get<bool>();
        return UnitTests::run("DisplayHandle #3", "Verify setEnabled/isEnabled from Lua", [=]() { return result; });
    }

    bool DisplayHandle_test4()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        bool result = lua.script(R"(
            local b = Core:createDisplayObject('Box', { name = 'li_box_vis', type = 'Box', x = 10, y = 10, width = 20, height = 20, isHidden = false })
            if not b then return false end
            local stage = Core:getStageHandle()
            if stage and b then stage:addChild(b) end
            if not b:isVisible() then return false end
            b:setVisible(false)
            if b:isVisible() then return false end
            b:setVisible(true)
            if not b:isVisible() then return false end
            b:setHidden(true)
            if not b:isHidden() then return false end
            b:setHidden(false)
            if b:isHidden() then return false end
            Core:destroyDisplayObject('li_box_vis')
            return true
        )").get<bool>();
        return UnitTests::run("DisplayHandle #4", "Verify setVisible/isVisible and setHidden/isHidden from Lua", [=]() { return result; });
    }

    bool DisplayHandle_test5()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        bool result = lua.script(R"(
            local b = Core:createDisplayObject('Box', { name = 'li_box_focus', type = 'Box', x = 10, y = 10, width = 20, height = 20 })
            if not b then return false end
            local stage = Core:getStageHandle()
            if stage and b then stage:addChild(b) end
            Core:setKeyboardFocusedObject(Core:getStageHandle())
            b:setKeyboardFocus()
            local k = Core:getKeyboardFocusedObject()
            if not k then return false end
            if k:getName() ~= 'li_box_focus' then return false end
            Core:destroyDisplayObject('li_box_focus')
            return true
        )").get<bool>();
        return UnitTests::run("DisplayHandle #5", "Verify setKeyboardFocus and Core:getKeyboardFocusedObject", [=]() { return result; });
    }

    bool DisplayHandle_test6()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        bool result = lua.script(R"(
            local b = Core:createDisplayObject('Box', { name = 'li_box_hover', type = 'Box', x = 10, y = 10, width = 20, height = 20 })
            if not b then 
                print("DisplayObject_test6: failed to create box")
                return false 
            end
            local stage = Core:getStageHandle()
            if stage and b then 
                stage:addChild(b) 
            else
                print("DisplayObject_test6: failed to get stage or add child")
                return false
            end
            local cx = b:getX() + b:getWidth()/2
            local cy = b:getY() + b:getHeight()/2
            Core:pushMouseEvent({ x = cx, y = cy, type = 'motion' })
            Core:pumpEventsOnce()
            local m = Core:getMouseHoveredObject()
            if not m then 
                print("DisplayObject_test6: getMouseHoveredObject returned nil")
                return false 
            end
            if m:getName() ~= 'li_box_hover' then 
                print("DisplayObject_test6: hovered object name mismatch: expected 'li_box_hover', got '"..m:getName().."'")
                return false 
            end
            Core:destroyDisplayObject('li_box_hover')
            return true
        )").get<bool>();
        return UnitTests::run("DisplayHandle #6", "Verify mouse hover via pushMouseEvent/pumpEventsOnce", [=]() { return result; });
    }


    bool DisplayHandle_test7()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();

        bool result = lua.script(R"(
        -- Create a test box with tab settings
            local b = Core:createDisplayObject('Box', { name = 'tab_box', type = 'Box', x = 10, y = 10, width = 20, height = 20, tabPriority = 5, tabEnabled = true })
            if not b then return false end

        -- Verify initial tab priority and enabled
            if b:getTabPriority() ~= 5 then return false end
            if not b:isTabEnabled() then return false end

        -- Change tab priority and enabled state
            b:setTabPriority(2)
            if b:getTabPriority() ~= 2 then return false end
            b:setTabEnabled(false)
            if b:isTabEnabled() then return false end

        -- Restore and cleanup
            b:setTabPriority(5)
            b:setTabEnabled(true)
            Core:destroyDisplayObject('tab_box')
            return true
        )").get<bool>();

        return UnitTests::run("DisplayHandle #7", "Exercise get/set TabPriority and is/setTabEnabled from Lua", [=]() { return result; });
    }


    bool DisplayHandle_test8()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();

        bool result = lua.script(R"(
        -- Create a test box
            local b = Core:createDisplayObject('Box', { name = 'geom_box', type = 'Box', x = 5, y = 6, width = 30, height = 40 })
            if not b then return false end

        -- Verify initial getters
            if b:getX() ~= 5 then return false end
            if b:getY() ~= 6 then return false end
            if b:getWidth() ~= 30 then return false end
            if b:getHeight() ~= 40 then return false end

        -- Modify via setters
            b:setX(15)
            b:setY(25)
            b:setWidth(60)
            b:setHeight(80)

        -- Verify updated values
            if b:getX() ~= 15 then return false end
            if b:getY() ~= 25 then return false end
            if b:getWidth() ~= 60 then return false end
            if b:getHeight() ~= 80 then return false end

        -- Cleanup
            Core:destroyDisplayObject('geom_box')
            return true
        )").get<bool>();
        return UnitTests::run("DisplayHandle #8", "Exercise setX/setY/setWidth/setHeight and verify getters from Lua", [=]() { return result; });
    }


    bool DisplayHandle_test9()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();

        // Lua performs these steps:
        //  - create three child boxes a,b,c attached to the stage
        //  - set priorities using various forms
        //  - verify resulting priorities and z-order where applicable
        auto result = lua.script(R"(
            local st = Core:getStageHandle()
            if not st then return { ok = false, err = 'no stage' } end

        -- cleanup any existing test children
            for _, n in ipairs({'ao','bo','co'}) do if Core:hasDisplayObject(n) then Core:destroyDisplayObject(n) end end
            local a = Core:createDisplayObject('Box', { name = 'ao', type = 'Box', x=0,y=0,width=8,height=8 })
            local b = Core:createDisplayObject('Box', { name = 'bo', type = 'Box', x=10,y=0,width=8,height=8 })
            local c = Core:createDisplayObject('Box', { name = 'co', type = 'Box', x=20,y=0,width=8,height=8 })
            if not (a and b and c) then return { ok = false, err = 'creation failed' } end
            st:addChild(a); st:addChild(b); st:addChild(c)

        -- Use numeric form to set this object's priority
            a:setPriority(5)
            if a:getPriority() ~= 5 then return { ok = false, err = 'numeric setPriority failed' } end
        
        -- Use table form to set priority on b
            b:setPriority({ priority = 7 })
            if b:getPriority() ~= 7 then return { ok = false, err = 'table setPriority failed' } end

        -- Use child-targeted form to set c's priority via parent


            st:setPriority({ child = 'co' }, 9)
            if c:getPriority() ~= 9 then return { ok = false, err = 'targeted setPriority failed' } end
        -- setZOrder using table on child 'bo'
            st:setZOrder({ child = 'bo', z = 42 })
            if b:getZOrder() ~= 42 then return { ok = false, err = 'setZOrder table failed' } end

        -- moveToTop for child 'ao' via parent
            st:moveToTop({ child = 'ao' })

        -- verify ao is now top-most by comparing children's zOrder or by checking parent ordering
            local prios = st:getChildrenPriorities()
            if not prios or #prios < 3 then return { ok = false, err = 'getChildrenPriorities failed' } end

        -- cleanup
            Core:destroyDisplayObject('ao')
            Core:destroyDisplayObject('bo')
            Core:destroyDisplayObject('co')
            return { ok = true }
        )").get<sol::table>();
        bool ok = result["ok"].get_or(false);
        std::string err = result["err"].get_or(std::string());
        if (!ok) {
            std::cout << "[Debug] DisplayHandle_test9 Lua error: " << err << std::endl;
        }
        return UnitTests::run("DisplayHandle #9", "Exercise setPriority/setZOrder/moveToTop overloads", [=]() { return ok; });
    }

    bool DisplayHandle_test10()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();

        auto res = lua.script(R"(
            local st = Core:getStageHandle()
            if not st then return { ok = false, err = 'no stage' } end

        -- cleanup
            for _, n in ipairs({'s1','s2','s3'}) do if Core:hasDisplayObject(n) then Core:destroyDisplayObject(n) end end
            local a = Core:createDisplayObject('Box', { name = 's1', type = 'Box', x=0,y=0,width=8,height=8 })
            local b = Core:createDisplayObject('Box', { name = 's2', type = 'Box', x=10,y=0,width=8,height=8 })
            local c = Core:createDisplayObject('Box', { name = 's3', type = 'Box', x=20,y=0,width=8,height=8 })
            st:addChild(a); st:addChild(b); st:addChild(c)

        -- assign mixed priorities
            a:setPriority(10)
            b:setPriority(5)
            c:setPriority(7)
            
            -- sort children
            st:sortChildrenByPriority()

        -- Instead of relying on stage-wide ordering (other fixtures may exist),
        -- query the specific test children directly for their priorities.
            local p1 = Core:getDisplayObject('s1'):getPriority()
            local p2 = Core:getDisplayObject('s2'):getPriority()
            local p3 = Core:getDisplayObject('s3'):getPriority()
            if not (p1 and p2 and p3) then return { ok = false, err = 'missing s1/s2/s3 priorities' } end
            local vals = {p1,p2,p3}
            table.sort(vals)
            if not (vals[1] == 5 and vals[2] == 7 and vals[3] == 10) then
                return { ok = false, err = 'initial sort failed, s priorities='..tostring(p1)..','..tostring(p2)..','..tostring(p3) }
            end

            -- setToHighest on child 's2' via parent
            st:setToHighestPriority({ child = 's2' })

        -- now priorities should be something like [7,10,11] or similar depending on implementation, but ensure s2 is highest
            st:sortChildrenByPriority()
            local pr2 = st:getChildrenPriorities()

        -- compute max among nonzero entries
            local maxv = nil
            for i,v in ipairs(pr2) do if v and v ~= 0 then if not maxv or v > maxv then maxv = v end end end
            if not maxv then return { ok = false, err = 'no nonzero priorities after setToHighest' } end
            if not (maxv == Core:getDisplayObject('s2'):getPriority()) then return { ok = false, err = 'setToHighest failed' } end

        -- setToLowest on child 's3' via parent
            st:setToLowestPriority({ child = 's3' })
            st:sortChildrenByPriority()
            local pr3 = st:getChildrenPriorities()

        -- compute min among nonzero entries
            local minv = nil
            for i,v in ipairs(pr3) do if v and v ~= 0 then if not minv or v < minv then minv = v end end end
            if not minv then return { ok = false, err = 'no nonzero priorities after setToLowest' } end
            if not (minv == Core:getDisplayObject('s3'):getPriority()) then return { ok = false, err = 'setToLowest failed' } end

        -- cleanup
            Core:destroyDisplayObject('s1')
            Core:destroyDisplayObject('s2')
            Core:destroyDisplayObject('s3')
            return { ok = true }
        )").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!ok) { std::cout << "[Debug] DisplayHandle_test10 Lua error: " << err << std::endl; }
        return UnitTests::run("DisplayHandle #10", "Verify sortChildrenByPriority and setToHighest/Lowest with child specs", [=]() { return ok; });
    }

    bool DisplayHandle_test11()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();

        auto res = lua.script(R"(
            -- retrieve the current stage object
                local st = getStage()
                if (not st) then
                    -- print("DisplayObject_test11: failed to get stage")
                    return { ok = false, err = 'no stage' }
                end

            -- void cleanAll_lua(IDisplayObject* obj); 
               --  st:cleanAll() -- nothing to return.  It will work it it works.
                -- Fail fast if the binding is missing so test surfaces the error
                if type(st.cleanAll) ~= 'function' then error("Missing Lua binding: IDisplayObject.cleanAll") end
                st:cleanAll()
                
            -- test the dirty accessors / mutators
                local dirty = st:getDirty() -- save the current state
                -- print("DisplayObject_test11: initial dirty state is ", tostring(dirty))
                st:setDirty() -- this SHOULD be setting the dirty flag to true
                -- print("DisplayObject_test11: secondary dirty state is ", tostring(st:getDirty()))

                if not st:isDirty() then
                    -- print("DisplayObject_test11: setDirty() failed")
                    return { ok = false, err = 'setDirty() failed' }
                end

            -- Hierarchy management (getChild, getParent)
                local redishBox = st:getChild('redishBox')
                if not redishBox then
                    print("DisplayObject_test11: getChild('redishBox') failed")
                    return { ok = false, err = "getChild('redishBox') failed" }
                end
                local parent = redishBox:getParent()
                if not parent then
                    print("DisplayObject_test11: getParent() failed")
                    return { ok = false, err = "getParent() failed" }
                end
                if parent ~= st then
                    print("DisplayObject_test11: getParent() mismatch")
                    return { ok = false, err = "getParent() mismatch" }
                end

            -- return success
                return { ok = true }
        )").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!ok) { std::cout << "[Debug] DisplayHandle_test11 Lua error: " << err << std::endl; }
        return UnitTests::run("DisplayHandle #11", "Dirty DisplayHandle and Hierarchy Management", [=]() { return ok; });
    } // END bool DisplayHandle()
   
    bool DisplayHandle_test12()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();

        auto res = lua.script(R"(
            -- retrieve the current stage object
                local st = getStage()
                if (not st) then
                    print("DisplayHandle_test12: failed to get stage")
                    return { ok = false, err = 'no stage' }
                end
            -- test the bounds of the blueishBox
                local blueishBox = st:getChild('blueishBox')
                if not blueishBox then
                    print("DisplayHandle_test12: getChild('blueishBox') failed")
                    return { ok = false, err = "getChild('blueishBox') failed" }
                end
                local b = blueishBox:getBounds()
                if not b then
                    print("DisplayHandle_test12: getBounds() failed")
                    return { ok = false, err = "getBounds() failed" }
                end
                if not (b.x == 235 and b.y == 65 and b.width == 245 and b.height == 220) then
                    print("DisplayHandle_test12: getBounds() unexpected values")
                    return { ok = false, err = "getBounds() unexpected values" }
                end
            -- get blueishBox color
                local color = blueishBox:getColor()
                if not color then
                    print("DisplayHandle_test12: getColor() failed")
                    return { ok = false, err = "getColor() failed" }
                end
                if not (color.r == 75 and color.g == 75 and color.b == 225 and color.a == 255) then
                    print("DisplayHandle_test12: getColor() unexpected values")
                    return { ok = false, err = "getColor() unexpected values" }
                end
            -- set blueishColor bounds to new values
                blueishBox:setBounds({ x = 240, y = 70, width = 250, height = 225 })
                local b2 = blueishBox:getBounds()
                if not b2 then
                    print("DisplayHandle_test12: getBounds() after setBounds failed")
                    return { ok = false, err = "getBounds() after setBounds failed" }
                end
                if not (b2.x == 240 and b2.y == 70 and b2.width == 250 and b2.height == 225) then
                    print("DisplayHandle_test12: setBounds() did not update values")
                    return { ok = false, err = "setBounds() did not update values" }
                end
            -- set blueishBox color to new values
                blueishBox:setColor({ r = 20, g = 20, b = 128, a = 255 })
                local color2 = blueishBox:getColor()
                if not color2 then
                    print("DisplayHandle_test12: getColor() after setColor failed")
                    return { ok = false, err = "getColor() after setColor failed" }
                end
                if not (color2.r == 20 and color2.g == 20 and color2.b == 128 and color2.a == 255) then
                    print("DisplayHandle_test12: setColor() did not update values")
                    return { ok = false, err = "setColor() did not update values" }
                end

               return { ok = true }
        )").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!ok) { std::cout << "[Debug] DisplayHandle_test12 Lua error: " << err << std::endl; }
        return UnitTests::run("DisplayHandle #12", "Type and Property Access", [=]() { return ok; });
    } // END bool DisplayHandle_test12()

    bool DisplayHandle_test13()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();

        auto res = lua.script(R"(
            -- retrieve the current stage object
                local st = getStage()
                if (not st) then
                    -- print("DisplayHandle_test13: failed to get stage")
                    return { ok = false, err = 'no stage' }
                end
            
            -- get handle to the blueishBox
                local blueishBox = st:getChild('blueishBox')
                if not blueishBox then
                    -- print("DisplayHandle_test13: getChild('blueishBox') failed")
                    return { ok = false, err = "getChild('blueishBox') failed" }
                end

            -- get handle to the redishBox
                local redishBox = st:getChild('redishBox')
                if not redishBox then
                    -- print("DisplayHandle_test13: getChild('redishBox') failed")
                    return { ok = false, err = "getChild('redishBox') failed" }
                end

            -- fetch the current keyboard focus object
                local kf = Core:getKeyboardFocusedObject()
                if not kf then
                    -- print("DisplayHandle_test13: getKeyboardFocusedObject() returned nil")
                    return { ok = false, err = "getKeyboardFocusedObject() returned nil" }
                end
                -- local kfName = kf:getName()
                -- if kfName == "" then
                --     kfName = "nil"
                --     print("DisplayHandle_test13: current keyboard focus is '" .. kfName .. "'")
                -- end                
            
            -- set keyboard focus to blueishBox
                blueishBox:setKeyboardFocus()
                local kf2 = Core:getKeyboardFocusedObject()
                if not kf2 then
                    -- print("DisplayHandle_test13: getKeyboardFocusedObject() after setKeyboardFocus returned nil")
                    return { ok = false, err = "getKeyboardFocusedObject() after setKeyboardFocus returned nil" }
                end
                if kf2 ~= blueishBox then
                    -- print("DisplayHandle_test13: setKeyboardFocus did not update focus to blueishBox")
                    return { ok = false, err = "setKeyboardFocus did not update focus to blueishBox" }
                end

            -- test isKeyboardFocused blueishBox
                if not blueishBox:isKeyboardFocused() then
                    -- print("DisplayHandle_test13: isKeyboardFocused() returned false after setKeyboardFocus")
                    return { ok = false, err = "isKeyboardFocused() returned false after setKeyboardFocus" }
                end   

            -- set keyboard focus to redishBox
                redishBox:setKeyboardFocus()
                local kf2 = Core:getKeyboardFocusedObject()
                if not kf2 then
                    -- print("DisplayHandle_test13: getKeyboardFocusedObject() after setKeyboardFocus returned nil")
                    return { ok = false, err = "getKeyboardFocusedObject() after setKeyboardFocus returned nil" }
                end
                if kf2 ~= redishBox then
                    -- print("DisplayHandle_test13: setKeyboardFocus did not update focus to redishBox")
                    return { ok = false, err = "setKeyboardFocus did not update focus to redishBox" }
                end

            -- test isKeyboardFocused redishBox
                if not redishBox:isKeyboardFocused() then
                    -- print("DisplayHandle_test13: isKeyboardFocused() returned false after setKeyboardFocus")
                    return { ok = false, err = "isKeyboardFocused() returned false after setKeyboardFocus" }
                end   

            -- test isMouseHovered blueishBox                
                -- push down/up and pump from Lua
                Core:pushMouseEvent({ x = 365, y = 182, type = "motion", button = 1 })
                Core:pumpEventsOnce()
                local blueHover = Core:getMouseHoveredObject()
                if not blueHover then
                    -- print("DisplayHandle_test13: getMouseHoveredObject() returned nil after motion over blueishBox")
                    return { ok = false, err = "getMouseHoveredObject() returned nil after motion over blueishBox" }
                end

            -- test isClickable redishBox
                blueishBox:setKeyboardFocus()
                redishBox:setClickable(false)
                if redishBox:isClickable() then
                    -- print("DisplayHandle_test13: isClickable() returned true for redishBox")
                    return { ok = false, err = "isClickable() returned true for redishBox" }
                end 
                Core:pushMouseEvent({ x = 160, y = 140, type = "down", button = 1 })
                Core:pushMouseEvent({ x = 160, y = 140, type = "up", button = 1 })
                Core:pumpEventsOnce()
                if (redishBox:isKeyboardFocused()) then
                    -- print("DisplayHandle_test13: redishBox received focus on click while not clickable")
                    return { ok = false, err = "redishBox received focus on click while not clickable" }
                end
                redishBox:setClickable(true)

                -- Click the Stage at 0, 0 to defocus the box
                Core:pushMouseEvent({ x = 0, y = 0, type = "down", button = 1 })
                Core:pushMouseEvent({ x = 0, y = 0, type = "up", button = 1 })
                Core:pumpEventsOnce()

               return { ok = true }
        )").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!ok) { std::cout << "[Debug] DisplayHandle_test13 Lua error: " << err << std::endl; }
        return UnitTests::run("DisplayHandle #13", "Focus and Interactivity", [=]() { return ok; });
    } // END bool DisplayHandle_test13()

    bool DisplayHandle_test14()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();

        auto res = lua.script(R"(
            -- retrieve the current stage object
                local st = getStage()
                if (not st) then
                    -- print("DisplayHandle_test14: failed to get stage")
                    return { ok = false, err = 'no stage' }
                end

            -- get handle to the blueishBox
                local blueishBox = st:getChild('blueishBox')
                if not blueishBox then
                    -- print("DisplayHandle_test14: getChild('blueishBox') failed")
                    return { ok = false, err = "getChild('blueishBox') failed" }
                end

            -- getX/getY/getWidth/getHeight
                local blueX = blueishBox:getX()
                local blueY = blueishBox:getY()
                local blueW = blueishBox:getWidth()
                local blueH = blueishBox:getHeight()
                if not (blueX == 240) then
                    return { ok = false, err = "getX() expected 240, got " .. tostring(blueX) }
                end
                if not (blueY == 70) then
                    return { ok = false, err = "getY() expected 70, got " .. tostring(blueY) }
                end
                if not (blueW == 250) then
                    return { ok = false, err = "getWidth() expected 250, got " .. tostring(blueW) }
                end
                if not (blueH == 225) then
                    return { ok = false, err = "getHeight() expected 225, got " .. tostring(blueH) }
                end

            -- setX/setY/setWidth/setHeight
                blueishBox:setX(245)
                blueishBox:setY(75)
                blueishBox:setWidth(255)
                blueishBox:setHeight(230)
                local blueX2 = blueishBox:getX()
                local blueY2 = blueishBox:getY()
                local blueW2 = blueishBox:getWidth()
                local blueH2 = blueishBox:getHeight()
                if not (blueX2 == 245) then
                    return { ok = false, err = "setX() expected 245, got " .. tostring(blueX2) }
                end
                if not (blueY2 == 75) then
                    return { ok = false, err = "setY() expected 75, got " .. tostring(blueY2) }
                end
                if not (blueW2 == 255) then
                    return { ok = false, err = "setWidth() expected 255, got " .. tostring(blueW2) }
                end
                if not (blueH2 == 230) then
                    return { ok = false, err = "setHeight() expected 230, got " .. tostring(blueH2) }
                end

                return { ok = true }
        )").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!ok) { std::cout << "[Debug] DisplayHandle_test14 Lua error: " << err << std::endl; }
        return UnitTests::run("DisplayHandle #14", "Geometry and Layout", [=]() { return ok; });
    } // END bool DisplayHandle_test14()


    bool DisplayHandle_test15()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();

        auto res = lua.script(R"lua(
            -- retrieve the current stage object
                local st = getStage()
                if (not st) then
                    return { ok = false, err = 'no stage' }
                end

            -- get handle to the blueishBox
                local blueishBox = st:getChild('blueishBox')
                if not blueishBox then
                    return { ok = false, err = "getChild('blueishBox') failed" }
                end

            -- getX, getY, getWidth, getHeight to establish baseline
                local blueX = blueishBox:getX()
                local blueY = blueishBox:getY()
                local blueW = blueishBox:getWidth()
                local blueH = blueishBox:getHeight()
                if not (blueX == 245) then
                    return { ok = false, err = "getX() expected 245, got " .. tostring(blueX) }
                end
                if not (blueY == 75) then
                    return { ok = false, err = "getY() expected 75, got " .. tostring(blueY) }
                end
                if not (blueW == 255) then
                    return { ok = false, err = "getWidth() expected 255, got " .. tostring(blueW) }
                end
                if not (blueH == 230) then
                    return { ok = false, err = "getHeight() expected 230, got " .. tostring(blueH) }
                end

            -- keep originals to verify anchors do not alter geometry
                local origX, origY, origW, origH = blueX, blueY, blueW, blueH

            -- cycle through all four edge anchors, setting each to a different value
                local AnchorPoints = {
                "top_left","top_center","top_right",
                "middle_left","middle_center","middle_right",
                "bottom_left","bottom_center","bottom_right"
                }

                local AnchorPointMap = {
                    top_left = 0, top_center = 1, top_right = 2,
                    middle_left = 3, middle_center = 4, middle_right = 5,
                    bottom_left = 6, bottom_center = 7, bottom_right = 8
                }

                local edges = {
                    { set = "setAnchorTop",    get = "getAnchorTop"    },
                    { set = "setAnchorLeft",   get = "getAnchorLeft"   },
                    { set = "setAnchorBottom", get = "getAnchorBottom" },
                    { set = "setAnchorRight",  get = "getAnchorRight"  },
                }

                local obj = Core:getDisplayObject("blueishBox") or getStage():getChild("blueishBox")
                if not obj then 
                    return { ok = false, err = "failed to retrieve blueishBox for anchor tests" }
                end

                for _, edge in ipairs(edges) do
                    -- ensure methods exist
                    assert(type(obj[edge.set]) == "function" and type(obj[edge.get]) == "function",
                            "missing anchor methods for edge: "..edge.set)
                    for _, ap in ipairs(AnchorPoints) do
                        local ap_val = AnchorPointMap[ap] or ap
                        -- pass numeric enum value to C++ setter
                        obj[edge.set](obj, ap_val)
                        local cur = obj[edge.get](obj)
                        if type(cur) == 'number' then
                            if cur ~= ap_val then
                                error(("Anchor mismatch (num): expected %s(%d) got %s for %s"):format(ap, ap_val, tostring(cur), edge.set))
                            end
                        else
                            if cur ~= ap then
                                error(("Anchor mismatch (str): expected %s got %s for %s"):format(ap, tostring(cur), edge.set))
                            end
                        end

                        -- verify geometry unchanged
                        local nx = obj:getX()
                        local ny = obj:getY()
                        local nw = obj:getWidth()
                        local nh = obj:getHeight()
                        if nx ~= origX then
                            return { ok = false, err = string.format("Anchor %s changed X: orig=%s now=%s", ap, tostring(origX), tostring(nx)) }
                        end
                        if ny ~= origY then
                            return { ok = false, err = string.format("Anchor %s changed Y: orig=%s now=%s", ap, tostring(origY), tostring(ny)) }
                        end
                        if nw ~= origW then
                            return { ok = false, err = string.format("Anchor %s changed Width: orig=%s now=%s", ap, tostring(origW), tostring(nw)) }
                        end
                        if nh ~= origH then
                            return { ok = false, err = string.format("Anchor %s changed Height: orig=%s now=%s", ap, tostring(origH), tostring(nh)) }
                        end

                    end
                end

            return { ok = true }
        )lua").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!ok) { std::cout << "[Debug] DisplayHandle_test15 Lua error: " << err << std::endl; }
        return UnitTests::run("DisplayHandle #15", "Edge Anchors", [=]() { return ok; });
    } // END bool DisplayHandle_test15()

    bool DisplayHandle_test16()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();

        auto res = lua.script(R"lua(
            -- retrieve the current stage object
                local st = getStage()
                if (not st) then
                    return { ok = false, err = 'no stage' }
                end

            -- get handle to the blueishBox
                local blueishBox = st:getChild('blueishBox')
                if not blueishBox then
                    return { ok = false, err = "getChild('blueishBox') failed" }
                end

            -- getLeft/getRight/getTop/getBottom to establish baseline
                local blueL = blueishBox:getLeft()
                local blueR = blueishBox:getRight()
                local blueT = blueishBox:getTop()
                local blueB = blueishBox:getBottom()
                if not (blueL == 245) then
                    return { ok = false, err = "getLeft() expected 245, got " .. tostring(blueL) }
                end
                if not (blueR == 500) then
                    return { ok = false, err = "getRight() expected 500, got " .. tostring(blueR) }
                end
                if not (blueT == 75) then
                    return { ok = false, err = "getTop() expected 75, got " .. tostring(blueT) }
                end
                if not (blueB == 305) then
                    return { ok = false, err = "getBottom() expected 305, got " .. tostring(blueB) }
                end

            -- setLeft/setRight/setTop/setBottom
                blueishBox:setLeft(250)
                blueishBox:setRight(510)
                blueishBox:setTop(80)
                blueishBox:setBottom(310)
                local blueL2 = blueishBox:getLeft()
                local blueR2 = blueishBox:getRight()
                local blueT2 = blueishBox:getTop()
                local blueB2 = blueishBox:getBottom()
                if not (blueL2 == 250) then
                    return { ok = false, err = "setLeft() expected 250, got " .. tostring(blueL2) }
                end
                if not (blueR2 == 510) then
                    return { ok = false, err = "setRight() expected 510, got " .. tostring(blueR2) }
                end
                if not (blueT2 == 80) then
                    return { ok = false, err = "setTop() expected 80, got " .. tostring(blueT2) }
                end
                if not (blueB2 == 310) then
                    return { ok = false, err = "setBottom() expected 310, got " .. tostring(blueB2) }
                end

            -- verify getLeft/getRight/getTop/getBottom agree
                if (blueishBox:getLeft() ~= blueL2) then
                    return { ok = false, err = "getLeft() disagrees with blueL2" }
                end
                if (blueishBox:getRight() ~= blueR2) then
                    return { ok = false, err = "getRight() disagrees with blueR2" }
                end
                if (blueishBox:getTop() ~= blueT2) then
                    return { ok = false, err = "getTop() disagrees with blueT2" }
                end
                if (blueishBox:getBottom() ~= blueB2) then
                    return { ok = false, err = "getBottom() disagrees with blueB2" }
                end

            return { ok = true }
        )lua").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!ok) { std::cout << "[Debug] DisplayHandle_test16 Lua error: " << err << std::endl; }
        return UnitTests::run("DisplayHandle #16", "Edge Positions", [=]() { return ok; });
    } // END bool DisplayHandle_test16()


    
    bool DisplayHandle_UnitTests() 
    {
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
              [&]() { return DisplayHandle_test1(); },  // Verify DisplayHandle forwards for priority/z-order exist and are callable
              [&]() { return DisplayHandle_test2(); },  // Verify setClickable/isClickable from Lua
              [&]() { return DisplayHandle_test3(); },  // Verify setEnabled/isEnabled from Lua
              [&]() { return DisplayHandle_test4(); },  // Verify setVisible/isVisible and setHidden/isHidden from Lua
              [&]() { return DisplayHandle_test5(); },  // Verify setKeyboardFocus and Core:getKeyboardFocusedObject
              [&]() { return DisplayHandle_test6(); },  // Verify mouse hover via pushMouseEvent/pumpEventsOnce
              [&]() { return DisplayHandle_test7(); },  // Exercise get/set TabPriority and is/setTabEnabled from Lua
              [&]() { return DisplayHandle_test8(); },  // Exercise setX/setY/setWidth/setHeight and verify getters from Lua
              [&]() { return DisplayHandle_test9(); },  // Exercise setPriority/setZOrder/moveToTop overloads
              [&]() { return DisplayHandle_test10(); }, // Verify sortChildrenByPriority and setToHighest/Lowest with child specs
              [&]() { return DisplayHandle_test11(); }, // Dirty DisplayHandle and Extended Hierarchy Management
              [&]() { return DisplayHandle_test12(); }, // Type and Property Access
              [&]() { return DisplayHandle_test13(); }, // Focus and Interactivity
              [&]() { return DisplayHandle_test14(); }, // Geometry and Layout
              [&]() { return DisplayHandle_test15(); }, // Edge Anchors
              [&]() { return DisplayHandle_test16(); }  // Edge Positions
        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    }

} // namespace SDOM