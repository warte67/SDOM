// scaffold_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>

namespace SDOM
{
    bool scaffold_test0()
    {
        std::string testName = "scaffold #0";
        std::string testDesc = "scaffold UnitTest Scaffolding";
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
    } // END: scaffold_test0()


    // --- Run the scaffold UnitTests --- //

    bool scaffold_UnitTests() 
    {
        std::string objName = "scaffold UnitTests";
        bool allTestsPassed = true;
        std::vector<std::function<bool()>> tests = 
        {
            [&]() { return scaffold_test0(); },    // scaffolding test

        };
        for (auto& test : tests) 
        {
            bool testResult = test();
            allTestsPassed &= testResult;
        }
        std::cout << CLR::indent() << CLR::LT_BLUE << "[" << objName << "] " << CLR::RESET;
        std::cout << (allTestsPassed ? CLR::GREEN + " [PASSED]" : CLR::fg_rgb(255, 0, 0) + " [FAILED]") << CLR::RESET << std::endl;          
        return allTestsPassed;
    } // END: scaffold_UnitTests()

} // END: namespace SDOM