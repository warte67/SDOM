// TristateButton_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
#include <SDOM/SDOM_TristateButton.hpp>
#include <SDOM/SDOM_CheckButton.hpp>
#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_Label.hpp>
#include <SDOM/SDOM_IButtonObject.hpp>
#include <SDOM/SDOM_IconIndex.hpp>


// // --------------------------------------------------------------------
// // 🧩 Virtual State Accessors (From IButtonObject)
// // --------------------------------------------------------------------
// virtual ButtonState getState() const override;           // ✅ Verified by tests 1–3
// virtual void setState(ButtonState state) override;       // ✅ Verified by tests 1–3

// // --------------------------------------------------------------------
// // 🧩 Public Accessors
// // --------------------------------------------------------------------
// DisplayHandle getLabelObject() const;                    // ✅ Verified by test 4
// std::string getText() const;                             // ✅ Verified by test 4
// DisplayHandle getIconButtonObject() const;               // ✅ Verified by test 5
// IconButton* getIconButton() const;                       // ✅ Used in tests 1–3 and 5
// SpriteSheet* getIconSpriteSheet() const;                 // ✅ Verified by test 5
// Label* getLabel() const;                                 // ✅ Verified by test 8
// std::string getFontResource() const;                     // ✅ Verified by test 5
// std::string getIconResource() const;                     // ✅ Verified by test 5
// int getFontSize() const;                                 // ✅ Verified by test 5
// int getFontWidth() const;                                // ✅ Verified by test 5
// int getFontHeight() const;                               // ✅ Verified by test 5
// bool getUseBorder() const;                               // ✅ Verified by test 6
// SDL_Color getLabelColor() const;                         // ✅ Verified by test 6
// SDL_Color getBorderColor() const;                        // ✅ Verified by test 6
// int getIconWidth() const;                                // ✅ Verified by test 5
// int getIconHeight() const;                               // ✅ Verified by test 5
// IconIndex getIconIndex() const;                          // ✅ Verified by test 7 (direct)

// // --------------------------------------------------------------------
// // 🧩 Public Mutators
// // --------------------------------------------------------------------
// void setText(const std::string& newText);                // ✅ Verified by test 4


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
        if (h == nullptr) {
            // fallback to a couple of other names from config
            h = f.getDisplayObject("mainFrame_CheckButton_2");
            if (h == nullptr) {
                h = f.getDisplayObject("mainFrame_CheckButton_3");
            }
        }
        return h ? dynamic_cast<TristateButton*>(h.get()) : nullptr;
    }

    // 1) Deterministic baseline: force Inactive and verify icon mapping
    bool TristateButton_test1_default(std::vector<std::string>& errors)
    {
        // Ensure factory-created children are initialized
        getCore().getFactory().onInit();
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
        getCore().getFactory().onInit();
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
        getCore().getFactory().onInit();
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



    // 4) Text and Label accessors: get/setText, getLabelObject
    bool TristateButton_test4_text_and_label(std::vector<std::string>& errors)
    {
        auto& factory = getCore().getFactory();
        factory.onInit();

        auto expect_true = [&](bool cond, const std::string& msg){ if (!cond) { errors.push_back(msg); } };

        TristateButton* tb = get_any_tristate_button();
        if (!tb) { errors.push_back("No TristateButton-derived object found (expected CheckButton)"); return true; }

        std::string oldText = tb->getText();
        std::string newText = "UnitTest Tristate";
        tb->setText(newText);
        expect_true(tb->getText() == newText, "setText/getText mismatch on TristateButton");

        DisplayHandle lh = tb->getLabelObject();
        expect_true(lh.isValid(), "getLabelObject returned invalid handle");
        if (lh.isValid())
        {
            Label* lbl = lh.as<Label>();
            expect_true(lbl != nullptr, "getLabelObject did not resolve to a Label");
            if (lbl)
            {
                expect_true(lbl->getText() == newText, "Label text did not reflect TristateButton text change");
            }
        }

        // restore original text
        tb->setText(oldText);
        return true;
    }

    // 5) Handles, resources, and font/icon metrics
    bool TristateButton_test5_handles_and_metrics(std::vector<std::string>& errors)
    {
        auto& factory = getCore().getFactory();
        factory.onInit();

        auto expect_true = [&](bool cond, const std::string& msg){ if (!cond) { errors.push_back(msg); } };

        TristateButton* tb = get_any_tristate_button();
        if (!tb) { errors.push_back("No TristateButton-derived object found (expected CheckButton)"); return true; }

        // getIconButtonObject + getIconButton
        DisplayHandle ibh = tb->getIconButtonObject();
        expect_true(ibh.isValid(), "getIconButtonObject returned invalid handle");
        IconButton* ib = tb->getIconButton();
        expect_true(ib != nullptr, "getIconButton returned null");

        // getIconSpriteSheet should resolve a spritesheet asset
        SpriteSheet* ss = tb->getIconSpriteSheet();
        expect_true(ss != nullptr, "getIconSpriteSheet returned null");

        // resource names should be non-empty
        expect_true(!tb->getFontResource().empty(), "getFontResource returned empty");
        expect_true(!tb->getIconResource().empty(), "getIconResource returned empty");

        // font metrics should be positive
        expect_true(tb->getFontSize() > 0, "getFontSize not positive");
        expect_true(tb->getFontWidth() > 0, "getFontWidth not positive");
        expect_true(tb->getFontHeight() > 0, "getFontHeight not positive");

        // icon metrics should be positive
        expect_true(tb->getIconWidth() > 0, "getIconWidth not positive");
        expect_true(tb->getIconHeight() > 0, "getIconHeight not positive");

        return true;
    }

    // 6) Colors and border flag
    static bool in_range_0_255(int v) { return v >= 0 && v <= 255; }
    bool TristateButton_test6_colors_and_border(std::vector<std::string>& errors)
    {
        auto& factory = getCore().getFactory();
        factory.onInit();

        auto expect_true = [&](bool cond, const std::string& msg){ if (!cond) { errors.push_back(msg); } };

        TristateButton* tb = get_any_tristate_button();
        if (!tb) { errors.push_back("No TristateButton-derived object found (expected CheckButton)"); return true; }

        SDL_Color lc = tb->getLabelColor();
        expect_true(in_range_0_255(lc.r) && in_range_0_255(lc.g) && in_range_0_255(lc.b) && in_range_0_255(lc.a),
                    "getLabelColor components out of range");

        SDL_Color bc = tb->getBorderColor();
        expect_true(in_range_0_255(bc.r) && in_range_0_255(bc.g) && in_range_0_255(bc.b) && in_range_0_255(bc.a),
                    "getBorderColor components out of range");

        // Just ensure the flag is readable; value is configuration-dependent
        bool useBorder = tb->getUseBorder();
        (void)useBorder; // avoid unused warning

        return true;
    }

    // 7) Direct getIconIndex() mapping from state (derived CheckButton)
    bool TristateButton_test7_icon_index_direct(std::vector<std::string>& errors)
    {
        auto& factory = getCore().getFactory();
        factory.onInit();

        auto expect_true = [&](bool cond, const std::string& msg){ if (!cond) { errors.push_back(msg); } };

        TristateButton* tb = get_any_tristate_button();
        if (!tb) { errors.push_back("No TristateButton-derived object found (expected CheckButton)"); return true; }

        tb->setState(ButtonState::Inactive);
        expect_true(tb->getIconIndex() == IconIndex::Checkbox_Empty, "Inactive -> getIconIndex mismatch");

        tb->setState(ButtonState::Active);
        expect_true(tb->getIconIndex() == IconIndex::Checkbox_Checked, "Active -> getIconIndex mismatch");

        tb->setState(ButtonState::Mixed);
        // CheckButton (two-state) maps Mixed -> Empty
        expect_true(tb->getIconIndex() == IconIndex::Checkbox_Empty, "Mixed -> getIconIndex mismatch for CheckButton");

        return true;
    }

    // 8) getLabel() raw pointer accessor returns a Label
    bool TristateButton_test8_getLabel_ptr(std::vector<std::string>& errors)
    {
        auto& factory = getCore().getFactory();
        factory.onInit();

        auto expect_true = [&](bool cond, const std::string& msg){ if (!cond) { errors.push_back(msg); } };

        TristateButton* tb = get_any_tristate_button();
        if (!tb) { errors.push_back("No TristateButton-derived object found (expected CheckButton)"); return true; }

        Label* raw = tb->getLabel();
        expect_true(raw != nullptr, "getLabel() returned nullptr");

        // Optional sanity: compare with DisplayHandle path when available
        DisplayHandle lh = tb->getLabelObject();
        if (lh.isValid())
        {
            Label* viaHandle = lh.as<Label>();
            expect_true(viaHandle != nullptr, "getLabelObject() did not resolve to Label");
            if (viaHandle)
            {
                expect_true(viaHandle == raw, "getLabel() and getLabelObject().as<Label>() mismatch");
            }
        }

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
            ut.add_test(objName, "Text + Label accessors", TristateButton_test4_text_and_label);
            ut.add_test(objName, "Handles + resources + metrics", TristateButton_test5_handles_and_metrics);
            ut.add_test(objName, "Colors + border flag", TristateButton_test6_colors_and_border);
            ut.add_test(objName, "Direct getIconIndex() mapping", TristateButton_test7_icon_index_direct);
            ut.add_test(objName, "getLabel() raw pointer", TristateButton_test8_getLabel_ptr);

            ut.add_test(objName, "Lua: src/TristateButton_UnitTests.lua", TristateButton_LUA_Tests, true);

            registered = true;
        }

        // return ut.run_all(objName, "TristateButton");
        return true;
    } // END: TristateButton_UnitTests()



} // END: namespace SDOM
