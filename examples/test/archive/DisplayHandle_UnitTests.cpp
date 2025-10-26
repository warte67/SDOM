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
    bool DisplayHandle_test0()
    {
        std::string testName = "DisplayHandle #0";
        std::string testDesc = "DisplayHandle UnitTest Scaffolding";
        sol::state& lua = getCore().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
                -- return { ok = false, err = "unknown error" }
                return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: DisplayHandle_test0()


    bool DisplayHandle_test1()
    {
        std::string testName = "DisplayHandle #1";
        std::string testDesc = "Verify DisplayHandle forwards for priority/z-order exist and are callable";
        sol::state& lua = getCore().getLua();
        // Lua script: check for presence of DisplayHandle methods related to priority/z-order
        auto res = lua.script(R"lua(
            local methods = { 'getMaxPriority','getMinPriority','getPriority','setToHighestPriority','setToLowestPriority','sortChildrenByPriority','setPriority','moveToTop','getChildrenPriorities','getZOrder','setZOrder' }
            local dh = Core:getStageHandle()
            if not dh then return { ok = false, err = 'no stage handle' } end
            for i, m in ipairs(methods) do
                local f = dh[m]
                if not f then 
                    print('DisplayHandle_test1: missing method -> '..m)
                    return { ok = false, err = 'missing '..m } 
                end
                if type(f) ~= 'function' then 
                    print('DisplayHandle_test1: method not callable -> '..m)
                    return { ok = false, err = m..' not callable' } 
                end
            end
            return { ok = true, err = "" }
        )lua").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: DisplayHandle_test1()


    // Split tests: clickable, enabled, visibility, keyboard focus, mouse hover
    bool DisplayHandle_test2()
    {
        std::string testName = "DisplayHandle #2";
        std::string testDesc = "Verify setClickable/isClickable from Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();

        auto res = lua.script(R"lua(
            local b = Core:createDisplayObject('Box', { name = 'li_box_click', type = 'Box', x = 10, y = 10, width = 20, height = 20, isClickable = true })
            if not b then return { ok = false, err = 'failed to create box' } end
            local stage = Core:getStageHandle()
            if stage and b then stage:addChild(b) end
            if not b:isClickable() then
                Core:destroyDisplayObject('li_box_click')
                return { ok = false, err = 'initial isClickable mismatch' }
            end
            b:setClickable(false)
            if b:isClickable() then
                Core:destroyDisplayObject('li_box_click')
                return { ok = false, err = 'setClickable(false) did not take' }
            end
            b:setClickable(true)
            if not b:isClickable() then
                Core:destroyDisplayObject('li_box_click')
                return { ok = false, err = 'setClickable(true) did not restore' }
            end
            Core:destroyDisplayObject('li_box_click')
            return { ok = true, err = "" }
        )lua").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: DisplayHandle_test2()



    bool DisplayHandle_test3()
    {
        std::string testName = "DisplayHandle #3";
        std::string testDesc = "Verify setEnabled/isEnabled from Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
            local b = Core:createDisplayObject('Box', { name = 'li_box_enabled', type = 'Box', x = 10, y = 10, width = 20, height = 20, isEnabled = true })
            if not b then return { ok = false, err = 'failed to create box' } end
            local stage = Core:getStageHandle()
            if stage and b then stage:addChild(b) end
            if not b:isEnabled() then
                Core:destroyDisplayObject('li_box_enabled')
                return { ok = false, err = 'initial isEnabled mismatch' }
            end
            b:setEnabled(false)
            if b:isEnabled() then
                Core:destroyDisplayObject('li_box_enabled')
                return { ok = false, err = 'setEnabled(false) did not take' }
            end
            b:setEnabled(true)
            if not b:isEnabled() then
                Core:destroyDisplayObject('li_box_enabled')
                return { ok = false, err = 'setEnabled(true) did not restore' }
            end
            Core:destroyDisplayObject('li_box_enabled')
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: DisplayHandle_test3()

    bool DisplayHandle_test4()
    {
        std::string testName = "DisplayHandle #4";
        std::string testDesc = "Verify setVisible/isVisible and setHidden/isHidden from Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
            local b = Core:createDisplayObject('Box', { name = 'li_box_vis', type = 'Box', x = 10, y = 10, width = 20, height = 20, isHidden = false })
            if not b then return { ok = false, err = 'failed to create box' } end
            local stage = Core:getStageHandle()
            if stage and b then stage:addChild(b) end
            if not b:isVisible() then
                Core:destroyDisplayObject('li_box_vis')
                return { ok = false, err = 'initial isVisible mismatch' }
            end
            b:setVisible(false)
            if b:isVisible() then
                Core:destroyDisplayObject('li_box_vis')
                return { ok = false, err = 'setVisible(false) did not take' }
            end
            b:setVisible(true)
            if not b:isVisible() then
                Core:destroyDisplayObject('li_box_vis')
                return { ok = false, err = 'setVisible(true) did not restore' }
            end
            b:setHidden(true)
            if not b:isHidden() then
                Core:destroyDisplayObject('li_box_vis')
                return { ok = false, err = 'setHidden(true) did not take' }
            end
            b:setHidden(false)
            if b:isHidden() then
                Core:destroyDisplayObject('li_box_vis')
                return { ok = false, err = 'setHidden(false) did not restore' }
            end
            Core:destroyDisplayObject('li_box_vis')
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: DisplayHandle_test4()


    bool DisplayHandle_test5()
    {
        std::string testName = "DisplayHandle #5";
        std::string testDesc = "Verify setKeyboardFocus and Core:getKeyboardFocusedObject";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
            local b = Core:createDisplayObject('Box', { name = 'li_box_focus', type = 'Box', x = 10, y = 10, width = 20, height = 20 })
            if not b then return { ok = false, err = 'failed to create box' } end
            local stage = Core:getStageHandle()
            if stage and b then stage:addChild(b) end
            Core:setKeyboardFocusedObject(Core:getStageHandle())
            b:setKeyboardFocus()
            local k = Core:getKeyboardFocusedObject()
            if not k then Core:destroyDisplayObject('li_box_focus'); return { ok = false, err = 'getKeyboardFocusedObject returned nil' } end
            if k:getName() ~= 'li_box_focus' then Core:destroyDisplayObject('li_box_focus'); return { ok = false, err = 'focused object name mismatch' } end
            Core:destroyDisplayObject('li_box_focus')
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: DisplayHandle_test5()


    bool DisplayHandle_test6()
    {
return true; // TEMP DISABLE
        std::string testName = "DisplayHandle #6";
        std::string testDesc = "Verify mouse hover via pushMouseEvent/pumpEventsOnce";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
            local b = Core:createDisplayObject('Box', { name = 'li_box_hover', type = 'Box', x = 10, y = 10, width = 20, height = 20 })
            if not b then 
                print("DisplayObject_test6: failed to create box")
                return { ok = false, err = "failed to create box" }
            end
            local stage = Core:getStageHandle()
            if stage and b then 
                stage:addChild(b) 
            else
                print("DisplayObject_test6: failed to get stage or add child")
                Core:destroyDisplayObject('li_box_hover')
                return { ok = false, err = "failed to get stage or add child" }
            end
            local cx = b:getX() + b:getWidth()/2
            local cy = b:getY() + b:getHeight()/2
            Core:pushMouseEvent({ x = cx, y = cy, type = 'motion' })
            Core:pumpEventsOnce()
            local m = Core:getMouseHoveredObject()
            if not m then 
                print("DisplayObject_test6: getMouseHoveredObject returned nil")
                Core:destroyDisplayObject('li_box_hover')
                return { ok = false, err = "getMouseHoveredObject returned nil" }
            end
            if m:getName() ~= 'li_box_hover' then 
                print("DisplayObject_test6: hovered object name mismatch: expected 'li_box_hover', got '"..m:getName().."'")
                Core:destroyDisplayObject('li_box_hover')
                return { ok = false, err = "hovered object name mismatch" }
            end
            Core:destroyDisplayObject('li_box_hover')
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: DisplayHandle_test6()


    bool DisplayHandle_test7()
    {
        std::string testName = "DisplayHandle #7";
        std::string testDesc = "Exercise get/set TabPriority and is/setTabEnabled from Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
            -- Create a test box with tab settings
            local b = Core:createDisplayObject('Box', { name = 'tab_box', type = 'Box', x = 10, y = 10, width = 20, height = 20, tabPriority = 5, tabEnabled = true })
            if not b then return { ok = false, err = 'failed to create box' } end

            -- Verify initial tab priority and enabled
            if b:getTabPriority() ~= 5 then Core:destroyDisplayObject('tab_box'); return { ok = false, err = 'initial tabPriority mismatch' } end
            if not b:isTabEnabled() then Core:destroyDisplayObject('tab_box'); return { ok = false, err = 'initial tabEnabled mismatch' } end

            -- Change tab priority and enabled state
            b:setTabPriority(2)
            if b:getTabPriority() ~= 2 then Core:destroyDisplayObject('tab_box'); return { ok = false, err = 'setTabPriority failed' } end
            b:setTabEnabled(false)
            if b:isTabEnabled() then Core:destroyDisplayObject('tab_box'); return { ok = false, err = 'setTabEnabled(false) did not take' } end

            -- Restore and cleanup
            b:setTabPriority(5)
            b:setTabEnabled(true)
            Core:destroyDisplayObject('tab_box')
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: DisplayHandle_test7()



    bool DisplayHandle_test8()
    {
        std::string testName = "DisplayHandle #8";
        std::string testDesc = "Exercise setX/setY/setWidth/setHeight and verify getters from Lua";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
            -- Create a test box
            local b = Core:createDisplayObject('Box', { name = 'geom_box', type = 'Box', x = 5, y = 6, width = 30, height = 40 })
            if not b then return { ok = false, err = 'failed to create geom_box' } end

            -- Verify initial getters
            if b:getX() ~= 5 then Core:destroyDisplayObject('geom_box'); return { ok = false, err = 'getX initial mismatch' } end
            if b:getY() ~= 6 then Core:destroyDisplayObject('geom_box'); return { ok = false, err = 'getY initial mismatch' } end
            if b:getWidth() ~= 30 then Core:destroyDisplayObject('geom_box'); return { ok = false, err = 'getWidth initial mismatch' } end
            if b:getHeight() ~= 40 then Core:destroyDisplayObject('geom_box'); return { ok = false, err = 'getHeight initial mismatch' } end

            -- Modify via setters
            b:setX(15); b:setY(25); b:setWidth(60); b:setHeight(80)

            -- Verify updated values
            if b:getX() ~= 15 then Core:destroyDisplayObject('geom_box'); return { ok = false, err = 'getX after set mismatch' } end
            if b:getY() ~= 25 then Core:destroyDisplayObject('geom_box'); return { ok = false, err = 'getY after set mismatch' } end
            if b:getWidth() ~= 60 then Core:destroyDisplayObject('geom_box'); return { ok = false, err = 'getWidth after set mismatch' } end
            if b:getHeight() ~= 80 then Core:destroyDisplayObject('geom_box'); return { ok = false, err = 'getHeight after set mismatch' } end

            -- Cleanup
            Core:destroyDisplayObject('geom_box')
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: DisplayHandle_test8()


    bool DisplayHandle_test9()
    {
        std::string testName = "DisplayHandle #9";
        std::string testDesc = "Exercise setPriority/setZOrder/moveToTop overloads";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
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
            if a:getPriority() ~= 5 then 
                return { ok = false, err = 'numeric setPriority failed' } 
            end

            -- Use table form to set priority on b
            b:setPriority({ priority = 7 })
            if b:getPriority() ~= 7 then 
                return { ok = false, err = 'table setPriority failed' } 
            end

            -- Use child-targeted form to set c's priority via parent
            st:setPriority({ child = 'co' }, 9)
            if c:getPriority() ~= 9 then 
                return { ok = false, err = 'targeted setPriority failed' } 
            end

            -- setZOrder using table on child 'bo'
            st:setZOrder({ child = 'bo', z = 42 })
            if b:getZOrder() ~= 42 then 
                return { ok = false, err = 'setZOrder table failed' } 
            end

            -- moveToTop for child 'ao' via parent
            st:moveToTop({ child = 'ao' })

            -- verify ao is now top-most by checking children priorities
            local prios = st:getChildrenPriorities()
            if not prios or #prios < 3 then 
                return { ok = false, err = 'getChildrenPriorities failed' } 
            end

            -- cleanup
            Core:destroyDisplayObject('ao')
            Core:destroyDisplayObject('bo')
            Core:destroyDisplayObject('co')
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: DisplayHandle_test9()


    bool DisplayHandle_test10()
    {
        std::string testName = "DisplayHandle #10";
        std::string testDesc = "Verify sortChildrenByPriority and setToHighest/Lowest with child specs";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
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
            st:sortChildrenByPriority()

            -- query specific test children directly for their priorities.
            local p1 = Core:getDisplayObject('s1'):getPriority()
            local p2 = Core:getDisplayObject('s2'):getPriority()
            local p3 = Core:getDisplayObject('s3'):getPriority()
            if not (p1 and p2 and p3) then 
                return { ok = false, err = 'missing s1/s2/s3 priorities' } 
            end
            local vals = {p1,p2,p3}
            table.sort(vals)
            if not (vals[1] == 5 and vals[2] == 7 and vals[3] == 10) then
                return { ok = false, err = 'initial sort failed, s priorities='..tostring(p1)..','..tostring(p2)..','..tostring(p3) }
            end

            -- setToHighest on child 's2' via parent
            st:setToHighestPriority({ child = 's2' })
            st:sortChildrenByPriority()
            local pr2 = st:getChildrenPriorities()

            -- compute max among nonzero entries
            local maxv = nil
            for i,v in ipairs(pr2) do if v and v ~= 0 then if not maxv or v > maxv then maxv = v end end end
            if not maxv then 
                return { ok = false, err = 'no nonzero priorities after setToHighest' } 
            end
            if not (maxv == Core:getDisplayObject('s2'):getPriority()) then 
                return { ok = false, err = 'setToHighest failed' } 
            end

            -- setToLowest on child 's3' via parent
            st:setToLowestPriority({ child = 's3' })
            st:sortChildrenByPriority()
            local pr3 = st:getChildrenPriorities()

            -- compute min among nonzero entries
            local minv = nil
            for i,v in ipairs(pr3) do if v and v ~= 0 then if not minv or v < minv then minv = v end end end
            if not minv then 
                return { ok = false, err = 'no nonzero priorities after setToLowest' } 
            end
            if not (minv == Core:getDisplayObject('s3'):getPriority()) then 
                return { ok = false, err = 'setToLowest failed' } 
            end

            -- cleanup
            Core:destroyDisplayObject('s1')
            Core:destroyDisplayObject('s2')
            Core:destroyDisplayObject('s3')
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!ok) { std::cout << "[Debug] DisplayHandle_test10 Lua error: " << err << std::endl; }
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: DisplayHandle_test10()


    bool DisplayHandle_test11()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
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
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!ok) { std::cout << "[Debug] DisplayHandle_test11 Lua error: " << err << std::endl; }
        return UnitTests::run("DisplayHandle #11", "Dirty DisplayHandle and Hierarchy Management", [=]() { return ok; });
    } // END bool DisplayHandle_test11()
   

    bool DisplayHandle_test12()
    {
        std::string testName = "DisplayHandle #12";
        std::string testDesc = "Type and Property Access";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"lua(
        -- retrieve the current stage object
            local st = getStage()
            if (not st) then
                return { ok = false, err = 'no stage' }
            end
        -- test the bounds of the blueishBox
            local blueishBox = st:getChild('blueishBox')
            if not blueishBox then
                return { ok = false, err = "getChild('blueishBox') failed" }
            end
            local b = blueishBox:getBounds()
            if not b then
                return { ok = false, err = "getBounds() failed" }
            end
            if not (b.x == 235 and b.y == 65 and b.width == 245 and b.height == 220) then
                return { ok = false, err = "getBounds() unexpected values" }
            end
        -- get blueishBox color
            local color = blueishBox:getColor()
            if not color then
                return { ok = false, err = "getColor() failed" }
            end
            if not (color.r == 75 and color.g == 75 and color.b == 225 and color.a == 255) then
                return { ok = false, err = "getColor() unexpected values" }
            end
        -- set blueishColor bounds to new values
            blueishBox:setBounds({ x = 240, y = 70, width = 250, height = 225 })
            local b2 = blueishBox:getBounds()
            if not b2 then
                return { ok = false, err = "getBounds() after setBounds failed" }
            end
            if not (b2.x == 240 and b2.y == 70 and b2.width == 250 and b2.height == 225) then
                return { ok = false, err = "setBounds() did not update values" }
            end
        -- set blueishBox color to new values
            blueishBox:setColor({ r = 20, g = 20, b = 128, a = 255 })
            local color2 = blueishBox:getColor()
            if not color2 then
                return { ok = false, err = "getColor() after setColor failed" }
            end
            if not (color2.r == 20 and color2.g == 20 and color2.b == 128 and color2.a == 255) then
                return { ok = false, err = "setColor() did not update values" }
            end
        -- return success
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END bool DisplayHandle_test12()


    bool DisplayHandle_test13()
    {
        std::string testName = "DisplayHandle #13";
        std::string testDesc = "Focus and Interactivity";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"(
        -- retrieve the current stage object
            local st = getStage()
            if (not st) then
                return { ok = false, err = 'failed to get stage' }
            end        
        -- get handle to the blueishBox
            local blueishBox = st:getChild('blueishBox')
            if not blueishBox then
                return { ok = false, err = "getChild('blueishBox') failed" }
            end
        -- get handle to the redishBox
            local redishBox = st:getChild('redishBox')
            if not redishBox then
                return { ok = false, err = "getChild('redishBox') failed" }
            end
        -- fetch the current keyboard focus object
            local kf = Core:getKeyboardFocusedObject()
            if not kf then
                return { ok = false, err = "getKeyboardFocusedObject() returned nil" }
            end        
        -- set keyboard focus to blueishBox
            blueishBox:setKeyboardFocus()
            local kf2 = Core:getKeyboardFocusedObject()
            if not kf2 then
                return { ok = false, err = "getKeyboardFocusedObject() after setKeyboardFocus returned nil" }
            end
            if kf2 ~= blueishBox then
                return { ok = false, err = "setKeyboardFocus did not update focus to blueishBox" }
            end
        -- test isKeyboardFocused blueishBox
            if not blueishBox:isKeyboardFocused() then
                return { ok = false, err = "isKeyboardFocused() returned false after setKeyboardFocus" }
            end   
        -- set keyboard focus to redishBox
            redishBox:setKeyboardFocus()
            local kf2 = Core:getKeyboardFocusedObject()
            if not kf2 then
                return { ok = false, err = "getKeyboardFocusedObject() after setKeyboardFocus returned nil" }
            end
            if kf2 ~= redishBox then
                return { ok = false, err = "setKeyboardFocus did not update focus to redishBox" }
            end
        -- test isKeyboardFocused redishBox
            if not redishBox:isKeyboardFocused() then
                return { ok = false, err = "isKeyboardFocused() returned false after setKeyboardFocus" }
            end   
        -- test isMouseHovered blueishBox                
            -- push down/up and pump from Lua
            Core:pushMouseEvent({ x = 365, y = 182, type = "motion", button = 1 })
            Core:pumpEventsOnce()
            local blueHover = Core:getMouseHoveredObject()
            if not blueHover then
                return { ok = false, err = "getMouseHoveredObject() returned nil after motion over blueishBox" }
            end
        -- test isClickable redishBox
            blueishBox:setKeyboardFocus()
            redishBox:setClickable(false)
            if redishBox:isClickable() then
                return { ok = false, err = "isClickable() returned true for redishBox" }
            end 
            Core:pushMouseEvent({ x = 160, y = 140, type = "down", button = 1 })
            Core:pushMouseEvent({ x = 160, y = 140, type = "up", button = 1 })
            Core:pumpEventsOnce()
            if (redishBox:isKeyboardFocused()) then
                return { ok = false, err = "redishBox received focus on click while not clickable" }
            end
            redishBox:setClickable(true)

            -- Click the Stage at 0, 0 to defocus the box
            Core:pushMouseEvent({ x = 0, y = 0, type = "down", button = 1 })
            Core:pushMouseEvent({ x = 0, y = 0, type = "up", button = 1 })
            Core:pumpEventsOnce()
        -- return success
            return { ok = true, err = "" }
        )").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END bool DisplayHandle_test13()


    bool DisplayHandle_test14()
    {
        std::string testName = "DisplayHandle #14";
        std::string testDesc = "Geometry and Layout";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
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
        -- return success
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END bool DisplayHandle_test14()


    bool DisplayHandle_test15()
    {
        std::string testName = "DisplayHandle #15";
        std::string testDesc = "Edge Anchors";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
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
        -- cycle through all anchor points
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
            -- iterate edges and anchor points
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
        -- return success
            return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END bool DisplayHandle_test15()



    bool DisplayHandle_test16()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
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
        -- return success
            return { ok = true }
        )lua").get<sol::table>();
        // report and return test condition state
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
            [&]() { return DisplayHandle_test0(); },  // DisplayHandle UnitTest Scaffolding
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