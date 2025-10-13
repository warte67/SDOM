// Label Unit Tests

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>



namespace SDOM
{

    bool Label_test0()
    {
        std::string testName = "Label #0";
        std::string testDesc = "Label UnitTest Scaffolding";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"(
            -- return { ok = false, err = "unknown error" }
            return { ok = true, err = "" }
        )").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test0()


    bool Label_test1()
    {
        std::string testName = "Label #1";
        std::string testDesc = "Internal resources have been created";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"(
            local icon_8x8 = getAssetObject("internal_icon_8x8")
            if (icon_8x8:isValid() == false) then
                return { ok = false, err = "'internal_icon_8x8' asset not found" }
            end
            local ttf = getAssetObject("internal_ttf_asset")
            if (ttf:isValid() == false) then
                return { ok = false, err = "'internal_ttf_asset' asset not found" }
            end            
            local font_8x8 = getAssetObject("internal_font_8x8")
            if (font_8x8:isValid() == false) then
                return { ok = false, err = "'internal_font_8x8' asset not found" }
            end
            local font_8x12 = getAssetObject("internal_font_8x12")
            if (font_8x12:isValid() == false) then
                return { ok = false, err = "'internal_font_8x12' asset not found" }
            end
            return { ok = true, err = "" }
        )").get<sol::table>();
        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    } // END: Label_test1()

    bool Font_test2()
    {
        // NOTE:
            // Factory Asset Object Registry:
            //   Name: internal_font_8x8_Texture_3, Type: Texture, Filename: internal_font_8x8
            //   Name: ut_bmp8_lua2, Type: SpriteSheet, Filename: internal_font_8x8
            //   Name: ut_bmp8_lua, Type: SpriteSheet, Filename: internal_font_8x8
            //   Name: internal_font_8x8_Texture_1, Type: Texture, Filename: internal_font_8x8
            //   Name: ut_bmp8, Type: SpriteSheet, Filename: internal_font_8x8
            //   Name: external_font_8x12_SpriteSheet, Type: SpriteSheet, Filename: /home/jay/Documents/GitHub/SDOM/examples/test/assets/font_8x12.png
            //   Name: external_font_8x12, Type: BitmapFont, Filename: /home/jay/Documents/GitHub/SDOM/examples/test/assets/font_8x12.png
            //   Name: external_font_8x8, Type: BitmapFont, Filename: /home/jay/Documents/GitHub/SDOM/examples/test/assets/font_8x8.png
            //   Name: /home/jay/Documents/GitHub/SDOM/examples/test/assets/font_8x8.png, Type: Texture, Filename: /home/jay/Documents/GitHub/SDOM/examples/test/assets/font_8x8.png
            //   Name: internal_ttf_asset, Type: TTFAsset, Filename: internal_ttf
            //   Name: VarelaRound32, Type: truetype, Filename: VarelaRound32_TTFAsset
            //   Name: internal_font_8x8, Type: BitmapFont, Filename: internal_font_8x8
            //   Name: /home/jay/Documents/GitHub/SDOM/examples/test/assets/font_8x12.png, Type: Texture, Filename: /home/jay/Documents/GitHub/SDOM/examples/test/assets/font_8x12.png
            //   Name: external_font_8x8_SpriteSheet, Type: SpriteSheet, Filename: /home/jay/Documents/GitHub/SDOM/examples/test/assets/font_8x8.png
            //   Name: internal_font_8x8_SpriteSheet, Type: SpriteSheet, Filename: internal_font_8x8
            //   Name: internal_font_8x8_Texture, Type: Texture, Filename: internal_font_8x8
            //   Name: internal_font_8x12, Type: BitmapFont, Filename: internal_font_8x12
            //   Name: VarelaRound16_TTFAsset, Type: TTFAsset, Filename: /home/jay/Documents/GitHub/SDOM/examples/test/assets/VarelaRound.ttf
            //   Name: internal_font_8x8_Texture_2, Type: Texture, Filename: internal_font_8x8
            //   Name: internal_font_8x12_SpriteSheet, Type: SpriteSheet, Filename: internal_font_8x12
            //   Name: internal_ttf, Type: truetype, Filename: internal_ttf_asset
            //   Name: VarelaRound16, Type: truetype, Filename: VarelaRound16_TTFAsset
            //   Name: internal_icon_8x8, Type: Texture, Filename: internal_icon_8x8
            //   Name: internal_font_8x12_Texture, Type: Texture, Filename: internal_font_8x12
            //   Name: VarelaRound32_TTFAsset, Type: TTFAsset, Filename: /home/jay/Documents/GitHub/SDOM/examples/test/assets/VarelaRound.ttf
            // Total asset objects: 25


        std::string testName = "Font #2";
        std::string testDesc = "SpriteSheet/BitmapFont asset existence and metrics";
        sol::state& lua = SDOM::Core::getInstance().getLua();
        // Lua test script
        auto res = lua.script(R"(
            return { ok = true, err = "" }
        )").get<sol::table>();

        // getFactory().printAssetRegistry();


        // report and return test condition state
        bool ok = res["ok"].get_or(false);
        std::string err = res["err"].get_or(std::string());
        if (!err.empty()) std::cout << CLR::ORANGE << "  [" << testName << "] " << err << CLR::RESET << std::endl;
        return UnitTests::run(testName, testDesc, [=]() { return ok; });

    } // END: Font_test2()


    bool Label_UnitTests() 
    {
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
            [&]() { return Label_test0(); },    // scaffolding test
            [&]() { return Label_test1(); },    // internal resources created
            [&]() { return Font_test2(); }      // SpriteSheet/BitmapFont asset existence and metrics
        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    }

} // namespace SDOM