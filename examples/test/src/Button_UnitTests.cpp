// Button_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
// Concrete types for assertions
#include <SDOM/SDOM_Button.hpp>
#include <SDOM/SDOM_Label.hpp>

namespace SDOM
{
    // --- Individual Button Unit Tests --- //

    // ============================================================================
    //  Test 0: Button Template
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
    bool Button_test0(std::vector<std::string>& errors)
    {
        // Example: To report an error, use this pattern:
        // errors.push_back("Description of the failure.");
        // ok = false;

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test

    } // END: Button_test0(std::vector<std::string>& errors)



    // ============================================================================
    //  Test 1: Button helpers (C++)
    // ----------------------------------------------------------------------------
    //  Validates getText/setText, getLabelColor/setLabelColor, getFontResource,
    //  and getLabelObject on a configured Button.
    // ============================================================================
    bool Button_test1(std::vector<std::string>& errors)
    {
        auto& core = getCore();
        auto& factory = core.getFactory();
        factory.onInit();

        auto expect_true = [&](bool cond, const std::string& msg){ if (!cond) errors.push_back(msg); };

        // Use a Button configured in examples/test/lua/config.lua
        DisplayHandle h = factory.getDisplayObject("main_stage_button");
        if (!h) { errors.push_back("main_stage_button not found"); return true; }
        Button* b = h.as<Button>();
        if (!b) { errors.push_back("main_stage_button is not a Button"); return true; }

        // getText/setText
        std::string oldText = b->getText();
        b->setText("UnitTest Button");
        expect_true(b->getText() == "UnitTest Button", "setText/getText mismatch");
        // restore to avoid visual diff across frames
        b->setText(oldText);

        // getLabelColor/setLabelColor
        SDL_Color c0 = b->getLabelColor();
        SDL_Color red{255,0,0,255};
        b->setLabelColor(red);
        SDL_Color c1 = b->getLabelColor();
        expect_true(c1.r == 255 && c1.g == 0 && c1.b == 0 && c1.a == 255, "setLabelColor/getLabelColor mismatch");
        // restore
        b->setLabelColor(c0);

        // getFontResource (expect configured name)
        std::string fr = b->getFontResource();
        expect_true(!fr.empty(), "getFontResource returned empty");

        // getLabelObject returns a valid Label handle
        DisplayHandle lh = b->getLabelObject();
        expect_true(lh.isValid(), "getLabelObject returned invalid handle");
        if (lh) {
            expect_true(lh.as<Label>() != nullptr, "getLabelObject did not resolve to a Label");
        }

        return true;
    } // END: Button_test1



    // --- Lua Integration Tests --- //

    bool Button_LUA_Tests(std::vector<std::string>& errors)
    {
        return UnitTests::getInstance().run_lua_tests(errors, "src/Button_UnitTests.lua");
    } // END: Button_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool Button_UnitTests()
    {
        const std::string objName = "Button";
        UnitTests& ut = UnitTests::getInstance();
        // ut.clear_tests();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Test Button", Button_test0);
            ut.add_test(objName, "Button helpers (C++)", Button_test1);

            ut.add_test(objName, "Lua: src/Button_UnitTests.lua", Button_LUA_Tests, true); // set to true to include lua test file

            registered = true;
        }

        // return ut.run_all(objName, "Button");
        return true;
    } // END: Button_UnitTests()

} // END: namespace SDOM
