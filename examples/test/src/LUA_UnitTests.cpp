// Lua_UnitTestss.cpp

// #ifdef SDOM_ENABLE_UNIT_TESTS

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp> 
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"

namespace SDOM
{
    DomHandle myBoxHandle;

    bool test1() {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        sol::object handle_obj = lua.script(R"(
            Core:createDisplayObject("Box", { 
                name = "myBox", 
                type = "Box", 
                x = 10, y = 20,
                width = 200, height = 100,
                color = { r = 96, g = 0, b = 96, a = 255 }
            })
            return Core:getDisplayHandle("myBox")
        )");
        // Try to get DomHandle from the returned object
        myBoxHandle = handle_obj.as<DomHandle>();
        bool box_exists = myBoxHandle.isValid();
        return UnitTests::run("Lua: test # 1", "Create Box and verify existence", [box_exists]() { return box_exists; });
    }


    bool test2() {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Pass the handle to Lua as a global
        lua["myBoxHandle"] = myBoxHandle;
        // Now Lua can use it to add to the stage
        bool added = lua.script(R"(
            local obj = myBoxHandle
            local stage = Core:getStageHandle()
            if not stage then return false end
            stage:addChild(obj)
            return stage:hasChild(obj)
        )").get<bool>();
        return UnitTests::run("Lua: test # 2", "Add the Box to Stage from Lua", [added]() { return added; });
    }

    bool test3() {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        bool destroyed = lua.script(R"(
            local boxHandle = Core:getDisplayHandle("myBox")
            local valid = boxHandle:isValid()
            if valid then
                Core:destroyDisplayObject("myBox")
                local inFactory = Core:hasDisplayObject("myBox")
                local stage = Core:getStageHandle()
                local inStage = false
                if stage then
                    inStage = stage:get():hasChild(boxHandle)
                end
                return not inFactory and not inStage
            end
            return false
        )").get<bool>();
        return UnitTests::run("Lua: test # 3", "Destroy Box and verify removal", [destroyed]() { return destroyed; });
    }

    // ----- Run all Lua unit tests -----

    bool LUA_UnitTests()
    {        
        bool allTestsPassed = true;

        // Vector of test functions
        std::vector<std::function<bool()>> tests = {
            [&]() { return test1(); },
            [&]() { return test2(); }
            // [&]() { return test3(); }


        };
        // Run each test and accumulate results
        for (auto& test : tests) {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        // Report test results
        return allTestsPassed;
    }
} // END namespace SDOM