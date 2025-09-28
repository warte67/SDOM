// Lua_UnitTestss.cpp

// #ifdef SDOM_ENABLE_UNIT_TESTS

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp> 
#include <SDOM/SDOM_UnitTests.hpp>
#include "UnitTests.hpp"

namespace SDOM
{
    bool test1() {
        return UnitTests::run("Lua: Test # 1", "Initial Lua Test", []() {
            return true;
        });
    }

    bool test2() {
        sol::state& lua = SDOM::Core::getInstance().getLua();
        bool handle_valid = lua.script(R"(
            local handle = Core:createDisplayObject("Box", { name = "myBox", type = "Box", x = 10, y = 20 })
            return handle ~= nil
        )").get<bool>();
        return UnitTests::run("Lua: test # 2", "Create Box from Lua", [handle_valid]() { return handle_valid; });
    }

    // ----- Run all Lua unit tests -----

    bool LUA_UnitTests()
    {
        bool allTestsPassed = true;

        // Vector of test functions
        std::vector<std::function<bool()>> tests = {
            [&]() { return test1(); },
            [&]() { return test2(); }
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