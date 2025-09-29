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

    bool test4() {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Create a temporary box from Lua, add to stage via Lua and verify
        bool ok = lua.script(R"(
            local h = Core:createDisplayObject("Box", { name = "tempBox", type = "Box", x = 1, y = 1, width = 16, height = 16 })
            if not h then return false end
            local stage = Core:getStageHandle()
            if not stage then return false end
            stage:addChild(h)
            local wasAdded = stage:hasChild(h)
            -- cleanup
            Core:destroyDisplayObject("tempBox")
            return wasAdded
        )").get<bool>();
        return UnitTests::run("Lua: test # 4", "Lua binding roundtrip add/hasChild", [ok]() { return ok; });
    }

    bool test5() {
        // std::vector<std::string> getPropertyNamesForType(const std::string& typeName) const;
        // std::vector<std::string> getCommandNamesForType(const std::string& typeName) const;
        // std::vector<std::string> getFunctionNamesForType(const std::string& typeName) const;

        sol::state& lua = SDOM::Core::getInstance().getLua();
        bool ok = lua.script(R"(
            -- Try a few possible bindings to the Factory/Core introspection API
            local names = nil
            if Core.getPropertyNamesForType then
                names = Core:getPropertyNamesForType("blueishBox")
            elseif Core.getFactory and Core.getFactory().getPropertyNamesForType then
                names = Core:getFactory():getPropertyNamesForType("blueishBox")
            elseif Factory and Factory.getPropertyNamesForType then
                names = Factory:getPropertyNamesForType("blueishBox")
            end

            if names and #names > 0 then
                print("Properties for 'blueishBox':")
                for i, n in ipairs(names) do
                    print("  " .. n)
                end
                return true
            else
                print("No property list available for 'blueishBox' or API not exposed to Lua.")
                return false
            end
        )").get<bool>();

        return UnitTests::run("Lua: test # 5", "blueishBox properties", [ok]() { return ok; });
    }

    // ----- Run all Lua unit tests -----

    bool LUA_UnitTests()
    {        
        bool allTestsPassed = true;

        // Vector of test functions
        std::vector<std::function<bool()>> tests = {
            [&]() { return test1(); },
            [&]() { return test2(); },
            [&]() { return test3(); },
            [&]() { return test4(); },
            [&]() { return test5(); } 
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