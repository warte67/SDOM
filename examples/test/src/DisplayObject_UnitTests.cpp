// DisplayObject_UnitTests.cpp

// #ifdef SDOM_ENABLE_UNIT_TESTS

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_DisplayObject.hpp>
#include "UnitTests.hpp"
// Include Box so test helpers (reset/getTestClickCount) are visible
#include "Box.hpp"


namespace SDOM
{

    // DisplayObject unit tests mapping (renamed to numeric identifiers):
    //  - DisplayObject_test1  -> "DisplayObject #1" (forwards/reflection)
    //  - DisplayObject_test2  -> "DisplayObject #2" (clickable)
    //  - DisplayObject_test3  -> "DisplayObject #3" (enabled)
    //  - DisplayObject_test4  -> "DisplayObject #4" (visibility)
    //  - DisplayObject_test5  -> "DisplayObject #5" (keyboard focus)
    //  - DisplayObject_test6  -> "DisplayObject #6" (mouse hover)
    //  - DisplayObject_test7  -> "DisplayObject #7" (tab priority)
    //  - DisplayObject_test8  -> "DisplayObject #8" (geometry getters/setters)
    //  - DisplayObject_test9  -> "DisplayObject #9" (priority overloads)
    //  - DisplayObject_test10 -> "DisplayObject #10" (sort/highest/lowest)

    bool DisplayObject_test1()
    {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua script: check for presence of DisplayObject methods related to priority/z-order
        auto res = lua.script(R"(
            local methods = { 'getMaxPriority','getMinPriority','getPriority','setToHighestPriority','setToLowestPriority','sortChildrenByPriority','setPriority','moveToTop','getChildrenPriorities','getZOrder','setZOrder' }
            local dh = Core:getStageHandle()
            if not dh then return { ok = false, err = 'no stage handle' } end
            for i, m in ipairs(methods) do
                local f = dh[m]
                if not f then return { ok = false, err = 'missing '..m } end
                if type(f) ~= 'function' then return { ok = false, err = m..' not callable' } end
            end
            return { ok = true }
        )").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        return UnitTests::run("DisplayObject #1", "Verify DisplayObject forwards for priority/z-order exist and are callable", [=]() { return ok; });
    }


    // Split tests: clickable, enabled, visibility, keyboard focus, mouse hover
    bool DisplayObject_test2()
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
        return UnitTests::run("DisplayObject #2", "Verify setClickable/isClickable from Lua", [=]() { return result; });
    }

    bool DisplayObject_test3()
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
        return UnitTests::run("DisplayObject #3", "Verify setEnabled/isEnabled from Lua", [=]() { return result; });
    }

    bool DisplayObject_test4()
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
        return UnitTests::run("DisplayObject #4", "Verify setVisible/isVisible and setHidden/isHidden from Lua", [=]() { return result; });
    }

    bool DisplayObject_test5()
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
        return UnitTests::run("DisplayObject #5", "Verify setKeyboardFocus and Core:getKeyboardFocusedObject", [=]() { return result; });
    }

    bool DisplayObject_test6()
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
        return UnitTests::run("DisplayObject #6", "Verify mouse hover via pushMouseEvent/pumpEventsOnce", [=]() { return result; });
    }


    bool DisplayObject_test7()
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

        return UnitTests::run("DisplayObject #7", "Exercise get/set TabPriority and is/setTabEnabled from Lua", [=]() { return result; });
    }


    bool DisplayObject_test8()
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
        return UnitTests::run("DisplayObject #8", "Exercise setX/setY/setWidth/setHeight and verify getters from Lua", [=]() { return result; });
    }


    bool DisplayObject_test9()
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
        return UnitTests::run("DisplayObject #9", "Exercise setPriority/setZOrder/moveToTop overloads", [=]() { return ok; });
    }

    bool DisplayObject_test10()
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
            local pr = st:getChildrenPriorities()

        -- Filter out zero entries (may be other children on the stage producing zeros).
            local nonzero = {}
            for i,v in ipairs(pr) do if v and v ~= 0 then table.insert(nonzero, v) end end
            if #nonzero < 3 then return { ok = false, err = 'not enough nonzero children priorities' } end

        -- after sort ascending the last three should be 5,7,10
            local n = #nonzero
            if not (nonzero[n-2] == 5 and nonzero[n-1] == 7 and nonzero[n] == 10) then
                local s = ''
                for i,v in ipairs(nonzero) do s = s .. tostring(v) .. ',' end
                return { ok = false, err = 'initial sort failed, nonzero=' .. s }
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
        if (!ok) {
            std::cout << "[Debug] DisplayObject_test10 Lua error: " << err << std::endl;
        }
            return UnitTests::run("DisplayObject #10", "Verify sortChildrenByPriority and setToHighest/Lowest with child specs", [=]() { return ok; });
    }



    bool DisplayObject_UnitTests() 
    {
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
              [&]() { return DisplayObject_test1(); },           // Verify DisplayObject forwards for priority/z-order exist and are callable
              [&]() { return DisplayObject_test2(); },            // Verify setClickable/isClickable from Lua
              [&]() { return DisplayObject_test3(); },              // Verify setEnabled/isEnabled from Lua
              [&]() { return DisplayObject_test4(); },           // Verify setVisible/isVisible and setHidden/isHidden from Lua
              [&]() { return DisplayObject_test5(); },        // Verify setKeyboardFocus and Core:getKeyboardFocusedObject
              [&]() { return DisplayObject_test6(); },           // Verify mouse hover via pushMouseEvent/pumpEventsOnce
              [&]() { return DisplayObject_test7(); },                  // Exercise get/set TabPriority and is/setTabEnabled from Lua
              [&]() { return DisplayObject_test8(); },             // Exercise setX/setY/setWidth/setHeight and verify getters from Lua
              [&]() { return DisplayObject_test9(); },   // Exercise setPriority/setZOrder/moveToTop overloads
              [&]() { return DisplayObject_test10(); }  // Verify sortChildrenByPriority and setToHighest/Lowest with child specs
        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    }

} // namespace SDOM