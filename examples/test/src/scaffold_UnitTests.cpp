// scaffold_UnitTests.cpp
#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IFontObject.hpp>

namespace SDOM
{
    // --- Individual Scaffold Unit Tests --- //

    // Test 0: Test scaffold
    bool scaffold_test0(std::vector<std::string>& errors)   
    {
        // ✅ Test Verified
        // 🔄 In Progress
        // ⚠️ Failing     
        // 🚫 Remove
        // ❌ Invalid
        // ☐ Planned

        bool ok = true;

        // To send an error message to the test harness, use the following:
        // errors.push_error("Description of the error.");
        // ok = false;

        return ok;
    } // scaffold_scaffolding(std::vector<std::string>& errors)   



    // --- Lua Integration Tests --- //

    bool scaffold_LUA_Tests(std::vector<std::string>& errors)
    {
        UnitTests& ut = UnitTests::getInstance();
        return ut.run_lua_tests(errors, ut.getLuaFilename());
    } // END: scaffold_LUA_Tests()


    // --- Main UnitTests Runner --- //
    bool scaffold_UnitTests()
    {
        UnitTests& ut = UnitTests::getInstance();
        ut.clear_tests();

        ut.add_test("Test scaffold", scaffold_test0);

        ut.setLuaFilename("src/scaffold_UnitTests.lua"); // Lua test script path
        ut.add_test("Lua: " + ut.getLuaFilename(), scaffold_LUA_Tests);


        return ut.run_all("scaffold");
    } // END: scaffold_UnitTests()



} // END: namespace SDOM