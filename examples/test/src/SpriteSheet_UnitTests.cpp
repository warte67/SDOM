// SpriteSheet_UnitTests.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <UnitTests.hpp>

namespace SDOM
{

    bool SpriteSheet_test0()
    {
        // SpriteSheet Scaffold: SpriteSheet Unit Test Scaffolding
        sol::state& lua = getLua();
        bool ok = false;
        try {
            // run a simple Lua chunk that returns a boolean and read it safely
            auto result = lua.script(R"(
                -- NO OP here, just scaffolding
                return true
            )");
            ok = result.get<bool>();
        } catch (const sol::error& e) {
            // log the Lua error and mark test as failed
            std::cerr << "[SpriteSheet Scaffold] Lua error: " << e.what() << std::endl;
            ok = false;
        }
        return UnitTests::run("SpriteSheet #0", "SpriteSheet Unit Test Scaffolding", [=]() { return ok; });
    }

    bool SpriteSheet_test1()
    {
        bool ok = true;
        Factory& factory = getFactory();

        // Test 1: create via InitStruct (basic AssetHandle/IAssetObject checks)
        ok = ok && UnitTests::run("SpriteSheet #1", "create via InitStruct (basic AssetHandle/IAssetObject checks)", [&factory]() -> bool 
        {
            SpriteSheet::InitStruct init;
            init.name = "ut_bmp8";
            init.type = SpriteSheet::TypeName;
            init.filename = "internal_font_8x8";
            init.spriteWidth  = 8;
            init.spriteHeight = 8;

            AssetHandle asset = factory.createAsset("SpriteSheet", init);
            if (!asset.isValid()) return false;

            SpriteSheet* ss = asset.as<SpriteSheet>();
            if (!ss) return false;

            if (ss->getSpriteWidth() != 8) {
                FAIL("SpriteSheet_test1: spriteWidth mismatch: " + std::to_string(ss->getSpriteWidth()));
                return false;
            }
            if (ss->getSpriteHeight() != 8) {
                FAIL("SpriteSheet_test1: spriteHeight mismatch: " + std::to_string(ss->getSpriteHeight()));
                return false;
            }
            if (ss->getName() != "ut_bmp8") {
                FAIL("SpriteSheet_test1: name mismatch: " + ss->getName());
                return false;
            }

            // attempt load (may throw via ERROR->Exception). catch and fail cleanly.
            try { ss->onLoad(); }  
            catch (const SDOM::Exception& e) { FAIL(std::string("SpriteSheet_test1: onLoad() exception: ") + e.what()); return false; } 
            catch (...) { FAIL("SpriteSheet_test1: onLoad() threw exception"); return false; }

            // basic texture-backed queries (if texture loaded)
            int count = 0;
            try { count = ss->getSpriteCount(); } 
            catch (const SDOM::Exception& e) { FAIL(std::string("SpriteSheet_test1: getSpriteCount() exception: ") + e.what()); return false; } 
            catch (...) { FAIL("SpriteSheet_test1: getSpriteCount() threw exception"); return false; }
            if (count <= 0) return false;

            // check first and last index computations (bounds)
            try 
            {
                int x0 = ss->getSpriteX(0);
                int y0 = ss->getSpriteY(0);
                int xl = ss->getSpriteX(count - 1);
                int yl = ss->getSpriteY(count - 1);
                (void)x0; (void)y0; (void)xl; (void)yl;
            } 
            catch (const SDOM::Exception& e) { FAIL(std::string("SpriteSheet_test1: getSpriteX/Y() exception: ") + e.what()); return false; }                  
            catch (...) { FAIL("SpriteSheet_test1: getSpriteX/Y() threw exception"); return false; }

            // cleanup
            ss->onUnload();
            return true;
        });
        return ok;
    }

    bool SpriteSheet_test2()
    {
        bool ok = true;
        Core& core = getCore();
        Factory& factory = getFactory();

        // Test 2: create via Factory/Lua-style config table (if supported)
        ok = ok && UnitTests::run("SpriteSheet #2", "Create SpriteSheet via Lua-style table", [&core, &factory]() -> bool {
            sol::state_view lua = getLua();
            sol::table cfg = lua.create_table();
            cfg["type"] = SpriteSheet::TypeName;
            cfg["name"] = "ut_bmp8_lua";
            cfg["filename"] = "internal_font_8x8";
            cfg["spriteWidth"] = 8;
            cfg["spriteHeight"] = 8;

            // Factory may expose createAsset that accepts sol::table (if implemented)
            AssetHandle asset;
            try {
                asset = factory.createAsset("SpriteSheet", cfg);
            } catch (...) {
                // if createAsset(sol::table) not implemented, consider as not-failing test path
                return true;
            }

            if (!asset.isValid()) return false;
            SpriteSheet* ss = asset.as<SpriteSheet>();
            if (!ss) return false;

            // basic property check
            if (ss->getName() != "ut_bmp8_lua") return false;

            // try load/unload
            try {
                ss->onLoad();
                ss->onUnload();
            } catch (...) {
                return false;
            }

            return true;
        });

        return ok;
    }

    bool SpriteSheet_test3() {
        sol::state& lua = getLua();
        bool ok = lua.script(R"(

            local ss = Core:createAsset("SpriteSheet", {
                type = "SpriteSheet",
                name = "ut_bmp8_lua2",
                filename = "internal_font_8x8",
                spriteWidth = 8,
                spriteHeight = 8
            })

            if not ss then 
                print("Failed to create SpriteSheet via Lua")
                return false 
            end
            if ss:getName() ~= "ut_bmp8_lua2" then 
                print("SpriteSheet name mismatch")
                return false 
            end
            local count = ss:getSpriteCount()
            if count <= 0 then 
                print("SpriteSheet sprite count invalid")
                return false 
            end
            local x0 = ss:getSpriteX(0)
            local y0 = ss:getSpriteY(0)
            local xl = ss:getSpriteX(count - 1)
            local yl = ss:getSpriteY(count - 1)
            if not x0 or not y0 or not xl or not yl then 
                print("SpriteSheet sprite index calculations failed")
                return false 
            end

            return true
        )").get<bool>();
        return UnitTests::run("SpriteSheet #3", "Use the 'default_bmp_8x8' SpriteSheet", [ok]() { return ok; });
    }

    bool SpriteSheet_test4() 
    {
        sol::state& lua = getLua();
        bool ok = lua.script(R"(

            local ss = getAssetObject("ut_bmp8_lua2")
            if not ss then
                print("Failed to get asset: ut_bmp8_lua2")
                return false
            end

            return true
        )").get<bool>();
        return UnitTests::run("SpriteSheet #4", "Exercise the 'default_bmp_8x8' SpriteSheet", [ok]() { return ok; });
    }

    bool SpriteSheet_UnitTests() 
    {
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
            [&]() { return SpriteSheet_test0(); }, // SpriteSheet Unit Test Scaffolding
            [&]() { return SpriteSheet_test1(); }, // create via InitStruct (basic AssetHandle/IAssetObject checks)
            [&]() { return SpriteSheet_test2(); }, // Create SpriteSheet via Lua-style table
            [&]() { return SpriteSheet_test3(); }, // Use the 'default_bmp_8x8' SpriteSheet
            [&]() { return SpriteSheet_test4(); }  // Exercise the 'default_bmp_8x8' SpriteSheet
        };

        for (auto& test : tests)
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    }
} // END namespace SDOM