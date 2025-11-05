// ArrowButton_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>

namespace SDOM
{
    // --- Individual ArrowButton Unit Tests --- //

    // ============================================================================
    //  Test 0: Template
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
    bool ArrowButton_test0(std::vector<std::string>& errors)
    {
        // Example: To report an error, use this pattern:
        // errors.push_back("Description of the failure.");
        // ok = false;

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test

    } // END: ArrowButton_test0(std::vector<std::string>& errors)




    // --- Lua Integration Tests --- //

    bool ArrowButton_LUA_Tests(std::vector<std::string>& errors)
    {
        return UnitTests::getInstance().run_lua_tests(errors, "src/ArrowButton_UnitTests.lua");
    } // END: ArrowButton_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool ArrowButton_UnitTests()
    {
        const std::string objName = "ArrowButton";
        UnitTests& ut = UnitTests::getInstance();
        // ut.clear_tests();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Test ArrowButton", ArrowButton_test0);

            ut.add_test(objName, "Lua: src/ArrowButton_UnitTests.lua", ArrowButton_LUA_Tests, true);  // not implemented yet (do not run)

            registered = true;
        }

        // return ut.run_all(objName, "ArrowButton");
        return true;
    } // END: ArrowButton_UnitTests()



} // END: namespace SDOM
