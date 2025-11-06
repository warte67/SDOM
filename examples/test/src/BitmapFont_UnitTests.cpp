// BitmapFont_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
// Concrete font and asset types
#include <SDOM/SDOM_BitmapFont.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>

namespace SDOM
{
    // --- Individual BitmapFont Unit Tests --- //

    // ============================================================================
    //  Test 0: BitmapFont Template
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
    bool BitmapFont_test0(std::vector<std::string>& errors)
    {
        // Example: To report an error, use this pattern:
        // errors.push_back("Description of the failure.");
        // ok = false;

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test

    } // END: BitmapFont_test0(std::vector<std::string>& errors)


    // ============================================================================
    //  Test 1: BitmapFont Lua-bound helpers (C++)
    // ----------------------------------------------------------------------------
    //  Validates getResourceHandle(), getBitmapFontWidth/Height(), and
    //  setBitmapFontWidth/Height() on the default internal bitmap font.
    // ============================================================================
    bool BitmapFont_test1(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        Factory& factory = core.getFactory();
        factory.onInit();

        auto expect_true = [&](bool cond, const std::string& msg){ if (!cond) errors.push_back(msg); };

        // Use the internal 8x8 bitmap font seeded by Factory::onInit
        AssetHandle h = factory.getAssetObject("internal_font_8x8");
        if (!h) { errors.push_back("Missing asset: internal_font_8x8"); return true; }
        if (h.getType() != std::string("BitmapFont")) {
            errors.push_back("Type mismatch for 'internal_font_8x8': got '" + h.getType() + "' expected 'BitmapFont'");
        }

        BitmapFont* bf = h.as<BitmapFont>();
        if (!bf) { errors.push_back("Failed dynamic_cast to BitmapFont for 'internal_font_8x8'"); return true; }

        // Initial metrics should reflect the internal font sprite size
        int w0 = bf->getBitmapFontWidth();
        int h0 = bf->getBitmapFontHeight();
        expect_true(w0 == 8, std::string("Expected initial BitmapFont width 8, got ") + std::to_string(w0));
        expect_true(h0 == 8, std::string("Expected initial BitmapFont height 8, got ") + std::to_string(h0));

        // Resource handle should be a SpriteSheet with matching tile size
        AssetHandle rs = bf->getResourceHandle();
        expect_true(bool(rs), "BitmapFont.getResourceHandle returned invalid handle");
        if (rs) {
            expect_true(rs.getType() == SpriteSheet::TypeName, std::string("Resource type mismatch: ") + rs.getType());
            if (auto* ss = rs.as<SpriteSheet>()) {
                expect_true(ss->getSpriteWidth() == 8 && ss->getSpriteHeight() == 8,
                            "SpriteSheet tile size mismatch for internal_font_8x8_SpriteSheet");
            }
        }

        // BitmapFont width/height are publicly immutable; they should match the SpriteSheet tile size
        if (auto* ss2 = rs.as<SpriteSheet>()) {
            expect_true(bf->getBitmapFontWidth()  == ss2->getSpriteWidth(),  "BitmapFont width != SpriteSheet tile width");
            expect_true(bf->getBitmapFontHeight() == ss2->getSpriteHeight(), "BitmapFont height != SpriteSheet tile height");
        }

        return true;
    } // END: BitmapFont_test1()



    // --- Lua Integration Tests --- //

    bool BitmapFont_LUA_Tests(std::vector<std::string>& errors)
    {
        return UnitTests::getInstance().run_lua_tests(errors, "src/BitmapFont_UnitTests.lua");
    } // END: BitmapFont_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool BitmapFont_UnitTests()
    {
        const std::string objName = "BitmapFont";
        UnitTests& ut = UnitTests::getInstance();
        // ut.clear_tests();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Test BitmapFont", BitmapFont_test0);
            ut.add_test(objName, "BitmapFont helpers (C++)", BitmapFont_test1);

            ut.add_test(objName, "Lua: src/BitmapFont_UnitTests.lua", BitmapFont_LUA_Tests, true);

            registered = true;
        }

        // return ut.run_all(objName, "BitmapFont");
        return true;
    } // END: BitmapFont_UnitTests()



} // END: namespace SDOM
