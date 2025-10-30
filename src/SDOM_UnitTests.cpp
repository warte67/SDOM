// SDOM_UnitTests.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_EventManager.hpp>

#include <SDOM/SDOM_UnitTests.hpp>  

namespace SDOM
{

    UnitTests& UnitTests::getInstance() 
    {
        static UnitTests instance;
        static bool lua_registered = false;
        if (!lua_registered) {
            instance.registerLua();
            lua_registered = true;
        }
        return instance;
    }

    void UnitTests::clear_tests() 
    {
        _tests.clear();
        _log.clear();
        _errors.clear();
    }    

    void UnitTests::add_test(const std::string& objName, const std::string& name, std::function<bool(std::vector<std::string>&)> func, bool is_implemented) 
    {
        TestCase testCase;
        testCase.obj_name = objName;
        testCase.name = name;
        testCase.func = func;
        testCase.is_implemented = is_implemented;
        testCase.has_run = false;
        testCase.passed = false;
        testCase.running = false;
        testCase.frame_count = 0;
        _tests.push_back(testCase);
        // _tests.push_back({name, func, is_implemented, false, false, false, 0});
    } // END add_test()

    bool UnitTests::run_all(const std::string& objName)
    {
        objName_ = objName;
        bool allPassed = true;

        for (auto& test : _tests)
            allPassed &= run_single_test(test, objName);

        return allPassed;
    }

    bool UnitTests::run_single_test(TestCase& test, const std::string& objName)
    {
        std::ostringstream oss;

        if (!test.is_implemented)
        {
            oss << CLR::indent() << CLR::NORMAL << "[" << objName << "] "
                << CLR::LT_BLUE << test.name << CLR::RESET
                << CLR::fg_rgb(224, 224, 64) + " [NOT IMPLEMENTED]" << CLR::RESET << std::endl;
            std::cout << oss.str();
            return true; // Skip so it doesn't block progress
        }

        std::vector<std::string> errors;
        bool finished = false; // means “this test is done (pass or fail)”
        bool passed   = false; // final outcome when finished == true

        try
        {
            // 🔹 Test function now returns “done?” instead of “passed?”
            finished = test.func(errors);
        }
        catch (const std::exception& e)
        {
            errors.push_back(e.what());
            finished = true;
        }

        // 🔹 If not finished yet, don’t print or finalize
        if (!finished)
            return false; // re-enter next frame

        // 🔹 Determine pass/fail based on error list
        passed = errors.empty();

        oss << CLR::indent() << CLR::NORMAL << "[" << objName << "] "
            << CLR::LT_BLUE << test.name << CLR::RESET
            << (passed ? CLR::GREEN + " [PASSED]" : CLR::fg_rgb(255, 0, 0) + " [FAILED]")
            << CLR::RESET << std::endl;

        if (!passed && !errors.empty())
        {
            for (const auto& line : errors)
                oss << CLR::indent() << CLR::fg_rgb(192, 64, 64)
                    << "    Error: " << line << std::endl;
            _tests_failed++;
        }

        std::cout << oss.str();
        return true; // means “this test is complete” (regardless of pass/fail)
    }



    void UnitTests::update()
    {
        if (_current_index >= static_cast<int>(_tests.size()))
            return;

        TestCase& test = _tests[_current_index];

        std::string frame_str = "Frame " + std::to_string(_frame_counter);
        frame_str.resize(10, ' ');

        std::cout << CLR::fg_rgb(128, 128, 255)
                << frame_str
                << CLR::RESET;

        // 🔹 Mark as running
        test.running = true;

        // 🔹 Execute one frame of this test
        bool finished = run_single_test(test, test.obj_name);

        // 🔹 If not finished, let it continue next frame
        if (!finished)
            return;

        // 🔹 Test finished — record its state
        test.has_run = true;
        test.running = false;
        test.frame_count = _frame_counter;

        // // 🔹 Determine pass/fail (from previous run_single_test pass/fail detection)
        // static int error_count = 0;
        // test.passed = (error_count == static_cast<int>(test.errors.size()));
        // all_passed_ &= test.passed;
        // error_count = static_cast<int>(test.errors.size());

        // 🔹 Determine pass/fail (compare with last frame’s errors)
        test.passed = (test.last_error_count == test.errors.size());
        all_passed_ &= test.passed;

        // 🔹 Update snapshot for next frame
        test.last_error_count = test.errors.size();        

        ++_current_index;

        // 🔹 If this was the last test, print summary
        if (_current_index >= static_cast<int>(_tests.size()))
        {
            int passed_count = 0;
            // int failed_count = 0;
            int not_implemented_count = 0;

            for (const auto& t : _tests)
            {
                if (!t.is_implemented)
                    ++not_implemented_count;
                if (t.passed)
                    ++passed_count;
                // else
                //     ++failed_count;
            }

            int total_count = static_cast<int>(_tests.size());

            if (_tests_failed == 0)
            {
                std::cout << CLR::fg_rgb(64, 255, 64)
                        << "✅ All unit tests passed."
                        << CLR::RESET << std::endl;
            }
            else
            {
                std::cout << CLR::fg_rgb(255, 0, 0)
                        << "❌ Some unit tests failed."
                        << CLR::RESET << std::endl;
            }

            std::cout << CLR::fg_rgb(255, 64, 64)
                    << "Summary: " << passed_count << "/" << total_count
                    << " tests passed, " << _tests_failed << " failed, "
                    << not_implemented_count << " not implemented."
                    << CLR::RESET << std::endl;

            if (_tests_failed > 0)
                shutdown();
        }
    }


    // --- UnitTests::run_lua_tests ---------------------------------------------------
    //
    // 🧩 Purpose:
    //   Executes a Lua-based unit test script and collects any reported errors.
    //   The Lua file is expected to return either:
    //     1. A table with optional fields: `ok` (bool) and `errors` (table of strings), or
    //     2. A single boolean value.
    //
    // 🧠 Notes:
    //   • The `ok` field and `errors` list are advisory; actual pass/fail is inferred
    //     by whether `errors` contains entries.
    //   • The function always returns `true` — signaling that Lua tests are one-shot.
    //   • Any Lua runtime or type errors are safely captured and logged.
    //
    // ⚠️ Safety:
    //   Uses `safe_script_file` with `sol::script_pass_on_error` to prevent Lua
    //   exceptions from propagating into C++. Ensures robust failure reporting.
    //
    // 🧭 TODO:
    //   • Refactor Lua test scripts to support **re-entrant** (multi-frame) testing,
    //     matching the updated SDOM unit test architecture.
    //   • Once refactored, this function should interpret a `false` return value as
    //     “continue next frame” instead of failure, maintaining consistency with
    //     the new C++ test pattern.    
    //
    // ============================================================================
    bool UnitTests::run_lua_tests(std::vector<std::string>& errors, const std::string& filename)
    {
        Core& core = getCore();
        sol::state& lua = core.getLua();

        // --- Execute Lua test script safely ---
        sol::protected_function_result result = lua.safe_script_file(filename, sol::script_pass_on_error);
        if (!result.valid())
        {
            sol::error err = result;
            errors.push_back(std::string("Lua runtime error: ") + err.what());
            return true; // ✅ one-shot test, completed this frame
        }

        sol::object return_value = result;

        // --- Case 1: Table return type ---
        if (return_value.is<sol::table>())
        {
            sol::table tbl = return_value.as<sol::table>();

            // Extract reported errors if present
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

            // Optional “ok” flag for human-readable summary — not used for logic
            bool reported_ok = tbl.get_or("ok", errors.empty());
            if (!reported_ok && errors.empty())
                errors.push_back("Lua test returned { ok = false } but no specific errors were provided.");
        }
        // --- Case 2: Boolean return type ---
        else if (return_value.is<bool>())
        {
            bool ok_flag = return_value.as<bool>();
            if (!ok_flag)
                errors.push_back("Lua test returned false.");
        }
        // --- Case 3: Invalid or unexpected type ---
        else
        {
            errors.push_back("Lua test did not return a valid result (expected table or boolean).");
        }

        return true; // ✅ always a one-shot test
    } // END: UnitTests::run_lua_tests(std::vector<std::string>& errors, const std::string& filename)



    // --- Core UnitTest Accessors / Mutators --- //

    SDL_Window* UnitTests::getWindow() { return getCore().window_; }
    SDL_Renderer* UnitTests::getRenderer() { return getCore().renderer_; }
    SDL_Texture* UnitTests::getTexture() { return getCore().texture_; }
    SDL_Color UnitTests::getColor() { return getCore().getColor(); }

    // --- Factory & EventManager Access --- //
    const Factory& UnitTests::getFactory() const { return *getCore().factory_; }
    EventManager& UnitTests::getEventManager() const { return *getCore().eventManager_; }
    bool UnitTests::getIsTraversing() const { return getCore().isTraversing_; }
    void UnitTests::setIsTraversing(bool traversing) { getCore().isTraversing_ = traversing; }   
    // --- Lua Registration --- //

    void UnitTests::registerLua()
    {
        /* Example LUA usage: 
            local ut = getUnitTests()
            ut:push_error("Something went wrong")
        */
        Core& core = getCore();
        sol::state& lua = core.getLua();
        if (!lua.lua_state()) {
            ERROR("Cannot register UnitTests Lua bindings: Lua state is not initialized.");
            return;
        }

        lua.new_usertype<UnitTests>("UnitTests"
            , "clear_tests", &UnitTests::clear_tests
            , "add_test", &UnitTests::add_test
            , "run_all", &UnitTests::run_all
        );
        lua.set_function("getUnitTests", &UnitTests::getInstance);

    } // END: registerLua()


    bool UnitTests::all_done() const 
    {
        bool anyRun = false;
        for (const auto& t : _tests)
        {
            if (t.is_implemented)
            {
                anyRun = true;
                if (!t.has_run)
                    return false;
            }
        }
        return anyRun; // true only if all implemented tests have has_run = true
    }


} // END: namespace SDOM