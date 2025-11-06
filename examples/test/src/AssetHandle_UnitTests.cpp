// AssetHandle_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>
// Concrete asset types used in checks
#include <SDOM/SDOM_Texture.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>
#include <SDOM/SDOM_TTFAsset.hpp>
#include <SDOM/SDOM_TruetypeFont.hpp>

namespace SDOM
{
    // --- Individual AssetHandle Unit Tests --- //

    // ============================================================================
    //  Test 0: AssetHandleing Template
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
    bool AssetHandle_test0(std::vector<std::string>& errors)
    {
        // Example: To report an error, use this pattern:
        // errors.push_back("Description of the failure.");
        // ok = false;

        // TODO: Add test logic here
        // e.g., if (!condition) { errors.push_back("Reason for failure."); ok = false; }

        return true; // ✅ finished this frame
        // return false; // 🔄 re-entrant test

    } // END: AssetHandle_test0(std::vector<std::string>& errors)



    // ============================================================================
    //  Test 1: Default resources created by Factory::onInit()
    // ----------------------------------------------------------------------------
    //  Verifies that the Factory seeds expected internal assets and that key
    //  invariants hold (types, basic properties). This is a one-shot test.
    // ============================================================================
    bool AssetHandle_test1(std::vector<std::string>& errors)
    {
        Core& core = getCore();
        Factory& factory = core.getFactory();
        // Idempotent; safe if already initialized
        factory.onInit();

        auto expect_asset = [&](const std::string& name, const std::string& type) {
            AssetHandle h = factory.getAssetObject(name);
            if (!h) {
                errors.push_back("Missing asset: '" + name + "'");
                return AssetHandle();
            }
            if (!type.empty() && h.getType() != type) {
                errors.push_back("Type mismatch for '" + name + "': got '" + h.getType() + "' expected '" + type + "'");
            }
            return h;
        };

        auto expect_true = [&](bool cond, const std::string& msg){ if (!cond) errors.push_back(msg); };

        // Internal icon textures
        {
            auto t8  = expect_asset("internal_icon_8x8",  Texture::TypeName);
            auto t12 = expect_asset("internal_icon_12x12", Texture::TypeName);
            auto t16 = expect_asset("internal_icon_16x16", Texture::TypeName);
            if (IAssetObject* p = t8.get())  expect_true(p->isLoaded(),  "Texture 'internal_icon_8x8' not loaded");
            if (IAssetObject* p = t12.get()) expect_true(p->isLoaded(),  "Texture 'internal_icon_12x12' not loaded");
            if (IAssetObject* p = t16.get()) expect_true(p->isLoaded(),  "Texture 'internal_icon_16x16' not loaded");
        }

        // SpriteSheet wrappers for internal icons with correct sprite sizes
        {
            auto ss8  = expect_asset("internal_icon_8x8_SpriteSheet",  SpriteSheet::TypeName);
            auto ss12 = expect_asset("internal_icon_12x12_SpriteSheet", SpriteSheet::TypeName);
            auto ss16 = expect_asset("internal_icon_16x16_SpriteSheet", SpriteSheet::TypeName);
            if (auto* p = ss8.as<SpriteSheet>())  { expect_true(p->getSpriteWidth() == 8 && p->getSpriteHeight() == 8,   "SpriteSheet size mismatch for internal_icon_8x8_SpriteSheet"); }
            if (auto* p = ss12.as<SpriteSheet>()) { expect_true(p->getSpriteWidth() == 12 && p->getSpriteHeight() == 12, "SpriteSheet size mismatch for internal_icon_12x12_SpriteSheet"); }
            if (auto* p = ss16.as<SpriteSheet>()) { expect_true(p->getSpriteWidth() == 16 && p->getSpriteHeight() == 16, "SpriteSheet size mismatch for internal_icon_16x16_SpriteSheet"); }
        }

        // Default bitmap fonts and their generated sprite sheets
        {
            // Factory registers/creates these with type "BitmapFont"
            auto bf88   = expect_asset("internal_font_8x8",  std::string("BitmapFont"));
            auto bf8x12 = expect_asset("internal_font_8x12", std::string("BitmapFont"));
            // SpriteSheets are created by BitmapFont::onInit under the names below
            auto s88    = expect_asset("internal_font_8x8_SpriteSheet",  SpriteSheet::TypeName);
            auto s8x12  = expect_asset("internal_font_8x12_SpriteSheet", SpriteSheet::TypeName);
            if (auto* p = s88.as<SpriteSheet>())   { expect_true(p->getSpriteWidth() == 8  && p->getSpriteHeight() == 8,  "SpriteSheet size mismatch for internal_font_8x8_SpriteSheet"); }
            if (auto* p = s8x12.as<SpriteSheet>()) { expect_true(p->getSpriteWidth() == 8  && p->getSpriteHeight() == 12, "SpriteSheet size mismatch for internal_font_8x12_SpriteSheet"); }
            if (IAssetObject* p = bf88.get())      expect_true(p->isLoaded(),  "BitmapFont 'internal_font_8x8' not loaded");
            if (IAssetObject* p = bf8x12.get())    expect_true(p->isLoaded(),  "BitmapFont 'internal_font_8x12' not loaded");
        }

        // Internal Truetype font chain
        {
            auto ttfAsset = expect_asset("internal_ttf_asset", TTFAsset::TypeName);
            if (auto* p = ttfAsset.as<TTFAsset>()) {
                expect_true(p->getFontSize() == 10, "TTFAsset 'internal_ttf_asset' font size != 10");
                expect_true(p->isLoaded(),          "TTFAsset 'internal_ttf_asset' not loaded");
            }

            // TruetypeFont wrapper can fail to load on systems without SDL_ttf; tolerate absence but check type if present
            AssetHandle tf = factory.getAssetObject("internal_ttf");
            if (tf) {
                if (tf.getType() != TruetypeFont::TypeName && tf.getType() != std::string("TruetypeFont")) {
                    errors.push_back("Type mismatch for 'internal_ttf': got '" + tf.getType() + "'");
                }
                if (auto* p = tf.as<TruetypeFont>()) {
                    // If TTFAsset loaded, TruetypeFont should report loaded as well
                    expect_true(p->isLoaded(), "TruetypeFont 'internal_ttf' not loaded");
                }
            }
        }

        return true; // Single frame
    } // END: AssetHandle_test1()



    // --- Lua Integration Tests --- //

    bool AssetHandle_LUA_Tests(std::vector<std::string>& errors)
    {
        return UnitTests::getInstance().run_lua_tests(errors, "src/AssetHandle_UnitTests.lua");
    } // END: AssetHandle_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool AssetHandle_UnitTests()
    {
        const std::string objName = "AssetHandle";
        UnitTests& ut = UnitTests::getInstance();
        // ut.clear_tests();

        static bool registered = false;
        if (!registered)
        {
            ut.add_test(objName, "Test AssetHandle", AssetHandle_test0);
            ut.add_test(objName, "Default resources from Factory::onInit", AssetHandle_test1);

            ut.add_test(objName, "Lua: src/AssetHandle_UnitTests.lua", AssetHandle_LUA_Tests, true);

            registered = true;
        }

        // return ut.run_all(objName, "AssetHandle");
        return true;
    } // END: AssetHandle_UnitTests()

} // END: namespace SDOM
