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
        bool ok = true;
        Core& core = getCore();
        sol::state& lua = core.getLua();

        std::string lua_file = "/home/jay/Documents/GitHub/SDOM/examples/test/src/Core_UnitTests.lua";

        // Run Lua script safely
        sol::protected_function_result result = lua.safe_script_file(lua_file, sol::script_pass_on_error);
        if (!result.valid())
        {
            sol::error err = result;
            errors.push_back(std::string("Lua runtime error: ") + err.what());
            return false;
        }

        // The script returns a value (hopefully a table)
        sol::object return_value = result;

        // If it’s a table, unpack it
        if (return_value.is<sol::table>())
        {
            sol::table tbl = return_value.as<sol::table>();

            // Extract ok flag (default to false if missing)
            ok = tbl.get_or("ok", false);

            // Extract the errors array (if present)
            sol::object err_field = tbl["errors"];
            if (err_field.is<sol::table>())
            {
                sol::table err_table = err_field.as<sol::table>();
                for (auto& kv : err_table)
                {
                    sol::object value = kv.second;
                    if (value.is<std::string>())
                        errors.push_back(value.as<std::string>());
                }
            }
        }
        else if (return_value.is<bool>())
        {
            // Script might just return a boolean (e.g. `return true`)
            ok = return_value.as<bool>();
        }
        else
        {
            // Unexpected return type
            errors.push_back("Lua test did not return a table or boolean.");
            ok = false;
        }

        return ok;
    } // END: scaffold_LUA_Tests()



    // --- Run the scaffold UnitTests --- //

   // --- Main Core UnitTests Runner --- //
    bool Core_UnitTests()
    {
        UnitTests& ut = UnitTests::getInstance();
        ut.clear_tests();

        ut.add_test("Scaffolding", scaffold_scaffolding);

        ut.add_test("Lua Integration", scaffold_LUA_Tests);

        return ut.run_all("Core");
    } // END: scaffold_UnitTests()



} // END: namespace SDOM