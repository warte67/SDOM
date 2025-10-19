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

    bool Group_test1()
    {
        std::string testName = "Group #1";
        std::string testDesc = "Create Group and verify basic bindings";
        sol::state& lua = SDOM::Core::getInstance().getLua();

        auto res = lua.script(R"lua(
            local g = createDisplayObject{ type = "Group", name = "ut_group_1" }
            return {
                ok = (g ~= nil),
                name = g and g.name or "",
                type = g and g.type or ""
            }
        )lua").get<sol::table>();

        bool ok = res["ok"].get_or(false);
        std::string nm = res["name"].get_or(std::string());
        std::string ty = res["type"].get_or(std::string());

        if (nm != "ut_group_1") ok = false;
        if (ty != "Group") ok = false;

        return UnitTests::run(testName, testDesc, [=]() { return ok; });
    }


    // --- Run the Group UnitTests --- //

    bool Group_UnitTests() 
    {
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
            [&]() { return Group_test0(); },   // UnitTest Scafolding
            [&]() { return Group_test1(); },   // Create Group and verify basic bindings
            // Add additional tests here as they are implemented
        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        return allTestsPassed;
    } // END: Group_UnitTests()

} // END: namespace SDOM

/*

- Group_test1() --  "Create and Bindings"
    - Purpose: create a Group from Lua (or factory), ensure handle exists and DisplayHandle methods call through.
    - Checks: group handle not nil, getName/getType work.

- Group_test2() -- Label property and function symmetry
    - Purpose: exercise label text & color via property-style and legacy function-style access.
    - Checks: set/read via both styles, round-trip equality (h.label_text / h:getLabelText()).

- Group_test3() -- Label pointer effects (C++-only verification)
    - Purpose: ensure C++ getLabelPtr() returns non-null for created group and that pointer changes are observed by handle accessors.
    - Checks: label pointer not null, set via pointer then read via Lua handle.

- Group_test4() -- Font metrics getters/setters (Lua symmetry)
    - Purpose: test getFontSize/getFontWidth/getFontHeight and setFont* via both property and function forms exposed on DisplayHandle.
    - Checks: setFontSize(…) then getFontSize() and h.font_size match.

- Group_test5() -- Spritesheet getters
    - Purpose: verify getSpriteSheet(), getSpriteWidth(), getSpriteHeight() return correct objects/sizes for internal_icon_* cases (8/12/16).
    - Checks: h:getSpriteSheet() is a handle, width/height match SpriteSheet tile size, and icon width in IPanelObject updated.

- Group_test6() -- Group color getters/setters
    - Purpose: test getGroupColor/setGroupColor round-trip via Lua property and function forms.
    - Checks: set to known RGBA, read back equal.

- Group_test7() -- Icon resource fallback behavior
    - Purpose: verify when config uses the texture name (internal_icon_12x12) the engine resolves and prefers the corresponding SpriteSheet wrapper.
    - Checks: getSpriteSheet().getFilename() or asset type indicates SpriteSheet and getSpriteWidth()==12.

- Group_test8() -- Edge cases and invalids
    - Purpose: ensure missing label/sprite gracefully returns defaults (empty text, color default, width/height 0) and no crash.
    - Checks: create Group with no icon_resource and assert getters return safe defaults.

*/
