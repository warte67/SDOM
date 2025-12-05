// IDisplayObject_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_Stage.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_Utils.hpp>


#include "Box.hpp"
#include <thread>
#include <chrono>

namespace SDOM
{
   // --- Individual IDisplayObject Unit Tests --- //

    // ============================================================================
    //  Test 0: Scaffolding Template
    // ----------------------------------------------------------------------------
    //  This template serves as a reference pattern for writing SDOM unit tests.
    //
    //  Status Legend:
    //   ✅ Test Verified     - Stable, validated, and passing
    //   🔄 In Progress       - Currently being implemented or debugged
    //   ⚠️  Failing          - Currently failing; requires investigation
    //   🚫 Remove            - Deprecated or replaced
    //   ❌ Invalid           - No longer applicable or test case obsolete
    //   ☐ Planned            - Placeholder for future implementation
    //
    //  Usage Notes:
    //   • To signal a test failure, push a descriptive message to `errors`.
    //   • Each test should return `true` once it has finished running.
    //   • Multi-frame tests may return `false` until all assertions pass.
    //   • Keep tests self-contained and deterministic.
    //
    // ============================================================================
    bool IDisplayObject_test0([[maybe_unused]] std::vector<std::string>& errors)
    {
        // Example: To report an error, use this pattern:
        // errors.push_back("Description of the failure.");
        bool done = true;

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        // return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test
        return done;

    } // END: IDisplayObject_test0(std::vector<std::string>& errors)


    // --- Lua Integration Tests --- //

    bool IDisplayObject_LUA_Tests(std::vector<std::string>& errors)
    {  
        return UnitTests::getInstance().run_lua_tests(errors, "src/IDisplayObject_UnitTests.lua");
    } // END: IDisplayObject_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool IDisplayObject_UnitTests()
    {
        const std::string objName = "IDisplayObject";
        UnitTests& ut = UnitTests::getInstance();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Scaffold", IDisplayObject_test0);

            // ut.add_test(objName, "Lua: 'src/IDisplayObject_UnitTests.lua'", IDisplayObject_LUA_Tests, false); 

            registered = true;
        }

        // return ut.run_all(objName);
        return true;
    } // END: IDisplayObject_UnitTests()



} // END: namespace SDOM
