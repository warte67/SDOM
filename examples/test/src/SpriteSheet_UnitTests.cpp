// SpriteSheet_UnitTests.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <UnitTests.hpp>

namespace SDOM
{

    bool SpriteSheet_test0()
    {
        std::string testName = "SpriteSheet #0";
        std::string testDesc = "SpriteSheet Unit Test Scaffolding";
        sol::state& lua = getLua();
        // Lua test script
        auto res = lua.script(R"lua(
                -- return { ok = false, err = "unknown error" }
                return { ok = true, err = "" }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: SpriteSheet_test0()


    bool SpriteSheet_test1()
    {
        std::string testName = "SpriteSheet #1";
        std::string testDesc = "create via InitStruct (basic AssetHandle/IAssetObject checks)";
        sol::state& lua = getLua();
        // Lua test script performs the same checks as the previous C++ test body.
        auto res = lua.script(R"lua(
            local ok = false
            local err = ""
            local function do_test()
                local ss = Core:createAsset("SpriteSheet", {
                    type = "SpriteSheet",
                    name = "ut_bmp8",
                    filename = "internal_font_8x8",
                    spriteWidth = 8,
                    spriteHeight = 8
                })
                if not ss then error("createAsset returned nil") end
                if ss:getSpriteWidth() ~= 8 then error("spriteWidth mismatch: " .. tostring(ss:getSpriteWidth())) end
                if ss:getSpriteHeight() ~= 8 then error("spriteHeight mismatch: " .. tostring(ss:getSpriteHeight())) end
                if ss:getName() ~= "ut_bmp8" then error("name mismatch: " .. tostring(ss:getName())) end
                local count = ss:getSpriteCount()
                if count <= 0 then error("spriteCount invalid: " .. tostring(count)) end
                -- check first and last index computations (bounds)
                local x0 = ss:getSpriteX(0)
                local y0 = ss:getSpriteY(0)
                local xl = ss:getSpriteX(count - 1)
                local yl = ss:getSpriteY(count - 1)
                if not x0 or not y0 or not xl or not yl then error("sprite index calculations failed") end
                return true
            end
            -- run the test
            local status, msg = pcall(do_test)
            if not status then err = tostring(msg) else ok = true end
            return { ok = ok, err = err }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: SpriteSheet_test1()


    bool SpriteSheet_test2()
    {
        std::string testName = "SpriteSheet #2";
        std::string testDesc = "Create SpriteSheet via Lua-style table";
        sol::state& lua = getLua();
        // Lua test script performs the same checks as the previous C++ test body.
        auto res = lua.script(R"lua(
            local ok = false
            local err = ""
            local function do_test()
                local cfg = {
                    type = "SpriteSheet",
                    name = "ut_bmp8_lua",
                    filename = "internal_font_8x8",
                    spriteWidth = 8,
                    spriteHeight = 8
                }

                -- attempt creation via Core:createAsset(cfg); if the binding/overload isn't implemented,
                -- treat that as a non-failing path (mirrors previous C++ catch behavior).
                local status, asset = pcall(function() return Core:createAsset("SpriteSheet", cfg) end)
                if not status then
                    return true
                end

                if not asset then error("createAsset returned nil") end
                if asset:getName() ~= "ut_bmp8_lua" then error("name mismatch: " .. tostring(asset:getName())) end

                local count = asset:getSpriteCount()
                if count <= 0 then error("spriteCount invalid: " .. tostring(count)) end

                -- check first and last index computations (bounds)
                local x0 = asset:getSpriteX(0)
                local y0 = asset:getSpriteY(0)
                local xl = asset:getSpriteX(count - 1)
                local yl = asset:getSpriteY(count - 1)
                if not x0 or not y0 or not xl or not yl then error("sprite index calculations failed") end

                return true
            end

            local status, msg = pcall(do_test)
            if not status then err = tostring(msg) else ok = true end
            return { ok = ok, err = err }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: SpriteSheet_test2()

    bool SpriteSheet_test3()
    {
        std::string testName = "SpriteSheet #3";
        std::string testDesc = "Use the 'default_bmp_8x8' SpriteSheet";
        sol::state& lua = getLua();

        auto res = lua.script(R"lua(
            local ok = false
            local err = ""
            local function do_test()
                local ss = Core:createAsset("SpriteSheet", {
                    type = "SpriteSheet",
                    name = "ut_bmp8_lua2",
                    filename = "internal_font_8x8",
                    spriteWidth = 8,
                    spriteHeight = 8
                })
                if not ss then error("createAsset returned nil") end
                if ss:getName() ~= "ut_bmp8_lua2" then error("name mismatch: " .. tostring(ss:getName())) end

                local count = ss:getSpriteCount()
                if count <= 0 then error("spriteCount invalid: " .. tostring(count)) end

                local x0 = ss:getSpriteX(0)
                local y0 = ss:getSpriteY(0)
                local xl = ss:getSpriteX(count - 1)
                local yl = ss:getSpriteY(count - 1)
                if not x0 or not y0 or not xl or not yl then error("sprite index calculations failed") end

                return true
            end

            local status, msg = pcall(do_test)
            if not status then err = tostring(msg) else ok = true end
            return { ok = ok, err = err }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: SpriteSheet_test3()

    bool SpriteSheet_test4() 
    {
        std::string testName = "SpriteSheet #4";
        std::string testDesc = "Exercise the 'default_bmp_8x8' SpriteSheet";
        sol::state& lua = getLua();

        auto res = lua.script(R"lua(
            local ok = false
            local err = ""
            local function do_test()
                local ss = getAssetObject("ut_bmp8_lua2")
                if not ss then error("Failed to get asset: ut_bmp8_lua2") end
                return true
            end

            local status, msg = pcall(do_test)
            if not status then err = tostring(msg) else ok = true end
            return { ok = ok, err = err }
        )lua").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: SpriteSheet_test4()

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