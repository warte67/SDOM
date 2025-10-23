// scaffold_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>

namespace SDOM
{
    // --- Individual Scaffold Unit Tests --- //
    bool scaffold_scaffolding(std::vector<std::string>& errors)   
    {
        bool ok = true;
        // Example test logic
        // if (some_condition_fails) {
        //     errors.push_back("Description of the failure.");
        //     ok = false;
        // }
        return ok;
    } // scaffold_scaffolding(std::vector<std::string>& errors)   



    // --- Lua Integration Tests --- //

    bool scaffold_LUA_Tests(std::vector<std::string>& errors)
    {
        UnitTests& ut = UnitTests::getInstance();
        return ut.run_lua_tests(errors, "src/scaffold_UnitTests.lua");
    } // END: scaffold_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool scaffold_UnitTests()
    {
        UnitTests& ut = UnitTests::getInstance();
        ut.clear_tests();

        ut.add_test("Scaffolding", scaffold_scaffolding);

        ut.add_test("Lua Integration", scaffold_LUA_Tests);

        return ut.run_all("Core");
    } // END: scaffold_UnitTests()



} // END: namespace SDOM