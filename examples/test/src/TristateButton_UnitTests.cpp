// TristateButton_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_TristateButton.hpp>
#include <SDOM/SDOM_CheckButton.hpp>
#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_IButtonObject.hpp>
#include <SDOM/SDOM_IconIndex.hpp>

namespace SDOM
{
    // --- Individual TristateButton Unit Tests --- //

    // ============================================================================
    //  Test 0: TristateButton Template
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
    bool TristateButton_test0(std::vector<std::string>& errors)
    {
        // Example: To report an error, use this pattern:
        // errors.push_back("Description of the failure.");
        // ok = false;

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test

    } // END: TristateButton_test0(std::vector<std::string>& errors)


    // Helper to get a CheckButton (inherits TristateButton) from configured scene
    static TristateButton* get_any_tristate_button(const char* preferredName = "mainFrame_CheckButton_1")
    {
        Factory& f = getCore().getFactory();
        DisplayHandle h = f.getDisplayObject(preferredName);
        if (!h) {
            // fallback to a couple of other names from config
            h = f.getDisplayObject("mainFrame_CheckButton_2");
            if (!h) h = f.getDisplayObject("mainFrame_CheckButton_3");
        }
        return h ? dynamic_cast<TristateButton*>(h.get()) : nullptr;
    }

    // 1) Deterministic baseline: force Inactive and verify icon mapping
    bool TristateButton_test1_default(std::vector<std::string>& errors)
    {
        TristateButton* tb = get_any_tristate_button();
        if (!tb) { errors.push_back("No TristateButton-derived object found (expected CheckButton)"); return true; }

        // Establish a known baseline regardless of external init order
        tb->setState(ButtonState::Inactive);
        auto st = tb->getState();
        if (st != ButtonState::Inactive) errors.push_back("Failed to set Inactive state baseline");

        // Icon mapping should reflect state
        IconButton* ib = tb->getIconButton();
        if (!ib) {
            errors.push_back("Internal IconButton missing");
            return true;
        }
        if (ib->getIconIndex() != IconIndex::Checkbox_Empty) errors.push_back("Inactive baseline icon mismatch (expected Empty)");
        return true;
    }

    // 2) Active state sets checked icon and state
    bool TristateButton_test2_active_toggle(std::vector<std::string>& errors)
    {
        TristateButton* tb = get_any_tristate_button();
        if (!tb) { errors.push_back("No TristateButton-derived object found (expected CheckButton)"); return true; }
        IconButton* ib = tb->getIconButton();
        if (!ib) { errors.push_back("Internal IconButton missing"); return true; }

        tb->setState(ButtonState::Active);
        if (tb->getState() != ButtonState::Active) errors.push_back("Failed to set Active state");
        if (ib->getIconIndex() != IconIndex::Checkbox_Checked) errors.push_back("Active state icon mismatch (expected Checked)");

        tb->setState(ButtonState::Inactive);
        if (tb->getState() != ButtonState::Inactive) errors.push_back("Failed to restore Inactive state");
        if (ib->getIconIndex() != IconIndex::Checkbox_Empty) errors.push_back("Inactive state icon mismatch (expected Empty)");
        return true;
    }

    // 3) Mixed state behavior: CheckButton maps Mixed -> Empty icon (two-state)
    bool TristateButton_test3_mixed_behavior(std::vector<std::string>& errors)
    {
        TristateButton* tb = get_any_tristate_button();
        if (!tb) { errors.push_back("No TristateButton-derived object found (expected CheckButton)"); return true; }
        IconButton* ib = tb->getIconButton();
        if (!ib) { errors.push_back("Internal IconButton missing"); return true; }

        tb->setState(ButtonState::Mixed);
        if (tb->getState() != ButtonState::Mixed) errors.push_back("Failed to set Mixed state");
        // For CheckButton, mixed shows empty icon
        if (ib->getIconIndex() != IconIndex::Checkbox_Empty) errors.push_back("Mixed state icon mismatch (expected Empty)");
        return true;
    }



    // --- Lua Integration Tests --- //

    bool TristateButton_LUA_Tests(std::vector<std::string>& errors)
    {
        return UnitTests::getInstance().run_lua_tests(errors, "src/TristateButton_UnitTests.lua");
    } // END: TristateButton_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool TristateButton_UnitTests()
    {
        const std::string objName = "TristateButton";
        UnitTests& ut = UnitTests::getInstance();
        // ut.clear_tests();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Test TristateButton", TristateButton_test0);
            ut.add_test(objName, "Default state + icon", TristateButton_test1_default);
            ut.add_test(objName, "Active toggle + icons", TristateButton_test2_active_toggle);
            ut.add_test(objName, "Mixed state behavior", TristateButton_test3_mixed_behavior);

            ut.add_test(objName, "Lua: src/TristateButton_UnitTests.lua", TristateButton_LUA_Tests, false);

            registered = true;
        }

        // return ut.run_all(objName, "TristateButton");
        return true;
    } // END: TristateButton_UnitTests()



} // END: namespace SDOM
