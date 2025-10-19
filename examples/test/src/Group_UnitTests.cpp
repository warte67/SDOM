// Group_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>

/**** Group UnitTests: **********************
 *  We need to ensure that all of these helpers are properly bound to LUA.  If the
 *  Lua functions work correctly, we then can assume the C++ versions work as well
 *  since the LUA functions are bound to the C++ versions.
 *  
 *
 *       // --- Label Helpers (C++ / LUA)--- //
 *
 *       DisplayHandle getLabel() const;                     // NOT YET TESTED
 *       std::string getLabelText() const;                   // NOT YET TESTED
 *       void setLabelText(const std::string& txt);          // NOT YET TESTED
 *       SDL_Color getLabelColor() const;                    // NOT YET TESTED
 *
 *       int getFontSize() const;                            // NOT YET TESTED
 *       int getFontWidth() const;                           // NOT YET TESTED
 *       int getFontHeight() const;                          // NOT YET TESTED
 *       void setFontSize(int s);                            // NOT YET TESTED
 *       void setFontWidth(int w);                           // NOT YET TESTED
 *       void setFontHeight(int h);                          // NOT YET TESTED
 *       void setLabelColor(SDL_Color c);                    // NOT YET TESTED
 *
 *
 *       // --- SpriteSheet Helpers (C++ / LUA)--- //        // NOT YET TESTED
 *
 *       AssetHandle getSpriteSheet() const;                 // NOT YET TESTED
 *       int getSpriteWidth() const;                         // NOT YET TESTED
 *       int getSpriteHeight() const;                        // NOT YET TESTED
 *       SDL_Color getGroupColor() const;                    // NOT YET TESTED
 *       void setGroupColor(const SDL_Color& c);             // NOT YET TESTED
 *
 * *********************************************/

namespace SDOM
{
    bool Group_test0()
    {
        std::string testName = "Group #0";
        std::string testDesc = "UnitTest Scafolding";
        sol::state& lua = SDOM::Core::getInstance().getLua();
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
    } // END: Group_test0()


    // --- Run the Group UnitTests --- //

    bool Group_UnitTests() 
    {
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
            [&]() { return Group_test0(); },   // UnitTest Scafolding
        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    } // END: Group_UnitTests()

} // END: namespace SDOM