// Version_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/CAPI/SDOM_CAPI_Version.h>

namespace SDOM
{
    // --- Individual Version Unit Tests --- //

    // ============================================================================
    //  Test 0: Version Template
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
    bool Version_test0([[maybe_unused]] std::vector<std::string>& errors)
    {
        // Example: To report an error, use this pattern:
        // errors.push_back("Description of the failure.");
        // bool done = false;

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        // To fetch the current frame:
        // int current_frame = UnitTests::getInstance().get_frame_counter();

        // Use State Machines when re-entrant tests are required to test SDOM main loop

        return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test

    } // END: Version_test0(std::vector<std::string>& errors)




    // =========================================================================
    //  Test: Validate Version C API surface against Core metadata
    // =========================================================================
    bool Version_CAPI_MetadataTest(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        bool done = true;

        auto checkString = [&](const char* actual,
                               const std::string& expected,
                               const char* label) {
            if (!actual)
            {
                errors.push_back(std::string(label) + " returned nullptr");
                done = true;
                return;
            }

            const std::string actualStr(actual);
            if (actualStr.empty())
            {
                errors.push_back(std::string(label) + " returned empty string");
                done = true;
                return;
            }

            if (actualStr != expected)
            {
                errors.push_back(
                    std::string(label) + " mismatch (expected '" + expected +
                    "', got '" + actualStr + "')");
                done = true;
            }
        };

        auto checkInt = [&](int actual, int expected, const char* label) {
            if (actual != expected)
            {
                errors.push_back(
                    std::string(label) + " mismatch (expected " +
                    std::to_string(expected) + ", got " +
                    std::to_string(actual) + ")");
                done = true;
            }
        };

        checkString(SDOM_GetVersionString(), core.getVersionString(),
                    "SDOM_GetVersionString");
        checkString(SDOM_GetVersionFullString(), core.getVersionFullString(),
                    "SDOM_GetVersionFullString");
        checkInt(SDOM_GetVersionMajor(), core.getVersionMajor(),
                 "SDOM_GetVersionMajor");
        checkInt(SDOM_GetVersionMinor(), core.getVersionMinor(),
                 "SDOM_GetVersionMinor");
        checkInt(SDOM_GetVersionPatch(), core.getVersionPatch(),
                 "SDOM_GetVersionPatch");
        checkString(SDOM_GetVersionCodename(), core.getVersionCodename(),
                    "SDOM_GetVersionCodename");
        checkString(SDOM_GetVersionBuild(), core.getVersionBuild(),
                    "SDOM_GetVersionBuild");
        checkString(SDOM_GetVersionBuildDate(), core.getVersionBuildDate(),
                    "SDOM_GetVersionBuildDate");
        checkString(SDOM_GetVersionCommit(), core.getVersionCommit(),
                    "SDOM_GetVersionCommit");
        checkString(SDOM_GetVersionBranch(), core.getVersionBranch(),
                    "SDOM_GetVersionBranch");
        checkString(SDOM_GetVersionCompiler(), core.getVersionCompiler(),
                    "SDOM_GetVersionCompiler");
        checkString(SDOM_GetVersionPlatform(), core.getVersionPlatform(),
                    "SDOM_GetVersionPlatform");

        return done;
    }




    // --- Lua Integration Tests --- //

    bool Version_LUA_Tests(std::vector<std::string>& errors)
    {
        return UnitTests::getInstance().run_lua_tests(errors, "src/Version_UnitTests.lua");
    } // END: Version_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool Version_UnitTests()
    {
        const std::string objName = "Version";
        UnitTests& ut = UnitTests::getInstance();
        // ut.clear_tests();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Test Version", Version_test0);
            ut.add_test(objName, "C API: Version metadata", Version_CAPI_MetadataTest);

            // ut.add_test(objName, "Lua: src/Version_UnitTests.lua", Version_LUA_Tests, false);

            registered = true;
        }

        // return ut.run_all(objName, "Version");
        return true;
    } // END: Version_UnitTests()



} // END: namespace SDOM
