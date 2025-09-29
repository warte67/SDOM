// Lua_UnitTests.cpp

// #ifdef SDOM_ENABLE_UNIT_TESTS

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"
// Lua_UnitTests.cpp

// #ifdef SDOM_ENABLE_UNIT_TESTS

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"

namespace SDOM {

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
    myBoxHandle = handle_obj.as<DomHandle>();
    bool box_exists = myBoxHandle.isValid();
    return UnitTests::run("Lua: test # 1", "Create Box and verify existence", [box_exists]() { return box_exists; });
}

bool test2() {
    sol::state& lua = SDOM::Core::getInstance().getLua();
    lua["myBoxHandle"] = myBoxHandle;
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
    bool ok = lua.script(R"(
        local h = Core:createDisplayObject("Box", { name = "tempBox", type = "Box", x = 1, y = 1, width = 16, height = 16 })
        if not h then return false end
        local stage = Core:getStageHandle()
        if not stage then return false end
        stage:addChild(h)
        local wasAdded = stage:hasChild(h)
        Core:destroyDisplayObject("tempBox")
        return wasAdded
    )").get<bool>();
    return UnitTests::run("Lua: test # 4", "Lua binding roundtrip add/hasChild", [ok]() { return ok; });
}

bool test5() {
    sol::state& lua = SDOM::Core::getInstance().getLua();
    auto pfr = lua.script(R"(
        local names = nil
        if Core.getPropertyNamesForType then
            names = Core:getPropertyNamesForType("blueishBox")
        elseif Core.getFactory and Core.getFactory().getPropertyNamesForType then
            names = Core:getFactory():getPropertyNamesForType("blueishBox")
        elseif Factory and Factory.getPropertyNamesForType then
            names = Factory:getPropertyNamesForType("blueishBox")
        end
        if names then
            local count = #names
            -- print(string.format("blueishBox property count: %d", count))
            if count > 2 then return true end
        end
        return false
    )");
    bool ok = pfr.get<bool>();
    return UnitTests::run("Lua: test # 5", "Verify getPropertyNamesForType(blueishBox)", [=]() { return ok; });
}

bool test6_Lua() {
    sol::state& lua = SDOM::Core::getInstance().getLua();
    auto pfr = lua.script(R"(
        local names = nil
        if Core.getFunctionNamesForType then
            names = Core:getFunctionNamesForType("blueishBox")
        elseif Core.getFactory and Core.getFactory().getFunctionNamesForType then
            names = Core:getFactory():getFunctionNamesForType("blueishBox")
        elseif Factory and Factory.getFunctionNamesForType then
            names = Factory:getFunctionNamesForType("blueishBox")
        end
        if names then
            local count = #names
            -- print(string.format("blueishBox function count: %d", count))
            if count > 0 then
                for i, n in ipairs(names) do
                    -- print(string.format("  %d: %s", i, n))
                end
                return true
            end
        end
        return false
    )");
    bool ok = pfr.get<bool>();
    return UnitTests::run("Lua: test # 6", "Verify getFunctionNamesForType(blueishBox)", [=]() { return ok; });
}

bool test7_Lua() {
    sol::state& lua = SDOM::Core::getInstance().getLua();
    auto pfr = lua.script(R"(
        local names = nil
        if Core.getCommandNamesForType then
            names = Core:getCommandNamesForType("blueishBox")
        elseif Core.getFactory and Core.getFactory().getCommandNamesForType then
            names = Core:getFactory():getCommandNamesForType("blueishBox")
        elseif Factory and Factory.getCommandNamesForType then
            names = Factory:getCommandNamesForType("blueishBox")
        end
        if names then
            local count = #names
            -- print(string.format("blueishBox command count: %d", count))
            -- Consider passing if zero or more; require >= 0 always true, but check API returns a table
            return true
        end
        return false
    )");
    bool ok = pfr.get<bool>();
    return UnitTests::run("Lua: test # 7", "Verify getCommandNamesForType(blueishBox)", [=]() { return ok; });
}

bool LUA_UnitTests() {
    bool allTestsPassed = true;
    std::vector<std::function<bool()>> tests = {
        [&]() { return test1(); },
        [&]() { return test2(); },
        [&]() { return test3(); },
        [&]() { return test4(); },
        [&]() { return test5(); },
        [&]() { return test6_Lua(); }
        ,[&]() { return test7_Lua(); }
    };
    for (auto& test : tests) {
        bool testResult = test();
        allTestsPassed &= testResult;
    }
    return allTestsPassed;
}

} // namespace SDOM