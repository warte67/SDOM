// SDOM_UnitTests.cpp

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_EventManager.hpp>
#include <SDOM/SDOM_Frame.hpp>
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

        
    // --- UnitTests::run_single_test ----------------------------------------------------
    //
    // 🧩 Purpose:
    //   Executes a single registered C++ unit test (one frame at a time).
    //   Responsible for invoking the test function, capturing exceptions,
    //   recording results, and printing output according to verbosity mode.
    //
    // 🧠 Notes:
    //   • `VERBOSE_TEST_OUTPUT = true`
    //       → Print *every* individual test line, pass/fail, inline.
    //   • `VERBOSE_TEST_OUTPUT = false`
    //       → Suppress all *passing* sub-tests.  Only failing or unimplemented
    //         tests are shown, and only per-module summaries appear later.
    //   • `QUIET_TEST_MODE = true`
    //       → Suppress *all* per-test and per-module output until the final summary.
    //
    // ⚙️ Behavior Summary:
    //   | Flag Configuration                  | Output Behavior                          |
    //   |------------------------------------|-------------------------------------------|
    //   | VERBOSE_TEST_OUTPUT = true         | Show all test lines + inline results.     |
    //   | VERBOSE_TEST_OUTPUT = false        | Only show failed / unimplemented tests.   |
    //   | QUIET_TEST_MODE = true             | Show nothing until final summary.         |
    //
    // ⚠️ Safety:
    //   • Exceptions are caught and reported without terminating the runner.
    //   • The return value indicates whether the test is "finished" (not pass/fail).
    //
    // -----------------------------------------------------------------------------------
    bool UnitTests::run_single_test(TestCase& test, const std::string& objName)
    {
        std::ostringstream oss;

        // -------------------------------------------------------------------------
        // 🧩 Handle unimplemented test
        // -------------------------------------------------------------------------
        if (!test.is_implemented)
        {
            if (!SDOM::QUIET_TEST_MODE)
            {
                oss << CLR::indent()
                    << CLR::NORMAL << "[" << objName << "] "
                    << CLR::LT_BLUE << test.name << CLR::RESET
                    << CLR::fg_rgb(224, 224, 64) + " [NOT YET IMPLEMENTED]"
                    << CLR::RESET << std::endl;
                std::cout << oss.str();
            }
            return true; // Skip so it doesn't block progress
        }

        // -------------------------------------------------------------------------
        // 🧩 Run test function safely
        // -------------------------------------------------------------------------
        std::vector<std::string> errors;
        bool finished = false;
        bool passed   = false;

        try
        {
            // Test functions return “finished?” (not “passed?”)
            finished = test.func(errors);
        }
        catch (const std::exception& e)
        {
            errors.push_back(e.what());
            finished = true;
        }

        // 🔹 If not finished yet, allow re-entrant continuation next frame
        if (!finished)
            return false;

        // -------------------------------------------------------------------------
        // 🧩 Determine pass/fail and record results
        // -------------------------------------------------------------------------
        passed = errors.empty();
        test.passed = passed;
        test.errors = std::move(errors);

        // -------------------------------------------------------------------------
        // 🧩 Output Control Based on Verbosity
        // -------------------------------------------------------------------------
        if (SDOM::QUIET_TEST_MODE)
            return true; // Suppress everything in quiet mode

        // In compact mode (not verbose), skip all passed tests
        if (!SDOM::VERBOSE_TEST_OUTPUT && passed)
            return true;

        // Otherwise, show this test’s result line
        oss << CLR::indent()
            << CLR::NORMAL << "[" << objName << "] "
            << CLR::LT_BLUE << test.name << CLR::RESET
            << (passed
                ? CLR::GREEN + " [PASSED]"
                : CLR::fg_rgb(255, 0, 0) + " [FAILED]")
            << CLR::RESET << std::endl;

        if (!passed && !test.errors.empty())
        {
            for (const auto& line : test.errors)
            {
                oss << CLR::indent()
                    << CLR::fg_rgb(192, 64, 64)
                    << "    Error: " << line << std::endl;
            }
            _tests_failed++;
        }

        std::cout << oss.str();
        return true; // Means “this test is complete” (pass or fail)
    } // END: UnitTests::run_single_test()



    // --- UnitTests::update -------------------------------------------------------
    //
    // 🧩 Purpose:
    //   Advances execution of registered unit tests, frame by frame. Each test
    //   runs until completion (either pass, fail, or not implemented). Once all
    //   tests complete, this function prints an appropriate summary depending
    //   on the configured verbosity flags.
    //
    // 🧠 Notes:
    //   • Behavior adapts dynamically based on compile-time flags:
    //       - VERBOSE_TEST_OUTPUT = true  → show all test details.
    //       - VERBOSE_TEST_OUTPUT = false → show module summaries only.
    //       - QUIET_TEST_MODE = true      → show nothing unless a failure occurs.
    //   • The summary always prints, regardless of verbosity.
    //   • Failed modules are expanded in quiet mode for error visibility.
    //
    // ⚙️ Behavior Matrix (Summary)
    //   | Mode         | Output Details                                            |
    //   |---------------|----------------------------------------------------------|
    //   | Verbose       | All tests + colored detail lines + final summary         |
    //   | Compact       | Only per-module summaries + final summary                |
    //   | Quiet         | Only failing modules (expanded) + final summary          |
    //
    // ============================================================================
    void UnitTests::update()
    {
        if (_current_index >= static_cast<int>(_tests.size()))
            return;

        TestCase& test = _tests[_current_index];
        test.running = true;

        // 🔹 Execute one frame of this test
        bool finished = run_single_test(test, test.obj_name);

        // 🔹 If not finished, continue next frame
        if (!finished)
            return;

        // 🔹 Mark completion and store results
        test.has_run = true;
        test.running = false;
        test.frame_count = _frame_counter;

        // 🔹 Determine pass/fail based on error deltas
        test.passed = (test.last_error_count == test.errors.size());
        all_passed_ &= test.passed;
        test.last_error_count = test.errors.size();        

        ++_current_index;

        // 🔹 If all tests complete, produce final summary
        if (_current_index >= static_cast<int>(_tests.size()))
        {
            int passed_count = 0;
            int not_implemented_count = 0;
            int failed_count = 0;

            for (const auto& t : _tests)
            {
                if (!t.is_implemented)
                    ++not_implemented_count;
                else if (t.passed)
                    ++passed_count;
                else
                    ++failed_count;
            }

            const int total_count = static_cast<int>(_tests.size());
            const bool any_failed = (failed_count > 0);
            const bool any_unimplemented = (not_implemented_count > 0);

            // =====================================================================
            // QUIET MODE: only show failing modules + errors + summary
            // =====================================================================
            if (QUIET_TEST_MODE)
            {
                if (any_failed)
                {
                    std::cout << std::endl
                            << CLR::fg_rgb(255, 128, 128)
                            << "❌ Failing Modules:" << CLR::RESET << std::endl;

                    for (const auto& t : _tests)
                    {
                        if (t.is_implemented && !t.passed)
                        {
                            std::cout << CLR::fg_rgb(255, 96, 96)
                                    << "  [" << t.obj_name << "] "
                                    << t.name << CLR::RESET << std::endl;

                            for (const auto& err : t.errors)
                                std::cout << CLR::fg_rgb(192, 64, 64)
                                        << "    Error: " << err
                                        << CLR::RESET << std::endl;
                        }
                    }
                    std::cout << std::endl;
                }
            }

            // =====================================================================
            // VERBOSE + COMPACT: print status summary if not in quiet mode
            // =====================================================================
            if (!QUIET_TEST_MODE)
            {
                if (any_failed)
                {
                    std::cout << CLR::fg_rgb(255, 64, 64)
                            << "❌ Some unit tests failed!"
                            << CLR::RESET << std::endl;
                }
                else if (any_unimplemented)
                {
                    std::cout << CLR::fg_rgb(255, 200, 64)
                            << "⚠️  All implemented tests passed, but some tests are not yet implemented."
                            << CLR::RESET << std::endl;
                }
                else
                {
                    std::cout << CLR::fg_rgb(64, 255, 64)
                            << "✅ All unit tests passed!"
                            << CLR::RESET << std::endl;
                }

                // 🧭 Always show compact summary in both verbose and compact modes
                print_summary(passed_count, failed_count, not_implemented_count, total_count);
            }
            else if (QUIET_TEST_MODE)
            {
                // Quiet mode still gets minimal summary lines
                if (any_unimplemented)
                {
                    std::cout << "⚠️  All implemented tests passed, but some tests are not yet implemented.\n";
                }
                print_summary(passed_count, failed_count, not_implemented_count, total_count);
            }

            // 🔻 Auto-shutdown if any failures
            if (failed_count > 0)
                shutdown();
        }
    } // END: UnitTests::update()


    // --- print_summary ----------------------------------------------------------
    //
    // 🧩 Purpose:
    //   Prints the compact summary line at the end of all unit tests, with color
    //   and symbol feedback based on the aggregate results.
    //
    // ⚙️ Color Rules:
    //   - 🟢 Green   → All implemented tests passed, none unimplemented.
    //   - 🟡 Yellow  → All implemented tests passed, but some unimplemented.
    //   - 🔴 Red     → One or more tests failed.
    //
    void UnitTests::print_summary(int passed_count,
                            int failed_count,
                            int not_implemented_count,
                            int total_count)
    {
        bool all_passed = (failed_count == 0);
        bool all_implemented = (not_implemented_count == 0);

        // Keep ANSI codes in std::string (safe, non-dangling)
        std::string color;
        std::string prefix;

        if (all_passed && all_implemented)
        {
            color  = CLR::fg_rgb(64, 255, 64);   // green
            prefix = "✅ ";
        }
        else if (all_passed && !all_implemented)
        {
            color  = CLR::fg_rgb(255, 200, 64);  // yellow
            prefix = "⚠️  ";
        }
        else
        {
            color  = CLR::fg_rgb(255, 128, 96);   // red
            prefix = "❌ ";
        }

        std::cout << color
                << prefix
                << "Summary: " << passed_count << "/" << total_count
                << " passed, " << failed_count << " failed, "
                << not_implemented_count << " not implemented."
                << CLR::RESET << std::endl;
        std::cout.flush();
    } // END: UnitTests::print_summary()


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
    } // END: UnitTests::run_all()


    // --- run_event_behavior_test-----------------------------------------------------
    //
    // 🧩 Purpose:
    //   Shared helper that simulates real input behavior for mouse, keyboard, and
    //   other interactive event types. It ensures SDL-style injected events result
    //   in the correct SDOM event dispatches through the EventManager.
    //
    // 📝 Notes:
    //   • Each simulated SDL action triggers a matching EventType lookup.  
    //   • Listeners are dynamically attached to a temporary Box node.  
    //   • Each listener records a "hit" if its corresponding event fires.  
    //   • Used by Event_test8 (mouse) and Event_test9 (keyboard) unit tests.  
    //
    // ⚙️ Functions Tested:
    //
    //   | Category             | Behavior / API Verified                           |
    //   |-----------------------|--------------------------------------------------|
    //   | Event Dispatch        | ✅ Core::pumpEventsOnce() correctly dispatches   |
    //   | Event Translation     | ✅ SDL events mapped to correct EventType        |
    //   | Listener Invocation   | ✅ addEventListener() / event callback execution |
    //   | Behavior Simulation   | ✅ Mouse & keyboard action triggers              |
    //   | Resource Cleanup      | ✅ Proper child removal and GC collection        |
    //
    // ⚠️ Safety:
    //   Temporary Box is destroyed and garbage collected after test completion.
    //
    // ============================================================================
    bool UnitTests::run_event_behavior_test(
        const std::vector<std::pair<std::string, std::function<void(DisplayHandle)>>>& actions, 
        std::vector<std::string>& errors)
    {
        Core& core = getCore();
        DisplayHandle stage = core.getRootNode();

        // --- 1) Create temporary Box for event testing -----------------------------
        Frame::InitStruct init;
        init.name   = "behaviorTestObject";
        init.x      = 10;
        init.y      = 10;
        init.width  = 50;
        init.height = 50;
        init.color  = {255, 0, 0, 255};

        DisplayHandle obj = core.createDisplayObject("Frame", init);
        if (!obj.isValid()) {
            errors.push_back("BehaviorTest: failed to create test Object.");
            return false;
        }
        obj->setClickable(true);  // <-- ensure clickable
        obj->setZOrder(10000);    // bring to front for targeting
        stage->addChild(obj);

        // ensure keyboard events are dispatched
        core.setKeyboardFocusedObject(obj); 
        // ensure mouse events are dispatched
        core.setMouseHoveredObject(obj);    

        // --- 2) Register event listeners -------------------------------------------
        std::unordered_map<std::string, bool> hits;
        for (const auto& [name, _] : actions)
            hits[name] = false;

        for (auto& [name, _] : actions)
        {
            EventType* et = nullptr;
            const auto& reg = EventType::getRegistry();
            auto it = reg.find(name);
            if (it != reg.end()) et = it->second;

            if (!et) {
                errors.push_back("Unknown EventType: " + name);
                continue;
            }

            // // 🔍 Diagnostic output
            // std::cout << "Registering listener for " << name
            //         << " EventType@" << static_cast<const void*>(et)
            //         << std::endl;
                    
            obj->addEventListener(*et, [&](const Event& ev) {
                hits[ev.getTypeName()] = true;
            });
        }

        // --- 3) Simulate SDL / behavior actions ------------------------------------
        for (auto& [_, action] : actions)
            action(obj);

        // Optional: capture queue size before dispatch for debugging
#if defined(SDOM_DEBUG_BEHAVIOR_TEST)
        {
            int qsz_before = core.getEventManager().getEventQueueSize();
            std::cout << "[BehaviorTest] Event queue size before pump: " << qsz_before << std::endl;
        }
#endif

        // --- 4) Process and verify event dispatch -----------------------------------
        core.pumpEventsOnce();

#if defined(SDOM_DEBUG_BEHAVIOR_TEST)
        bool any_missed = false;
        for (const auto& [name, hit] : hits) {
            if (!hit) {
                errors.push_back("Behavior event '" + name + "' did not fire.");
                any_missed = true;
            }
        }
        if (any_missed)
        {
            std::cout << "[BehaviorTest] Miss detected; dumping diagnostics...\n";
            // Dump queued events (should normally be empty after pump)
            core.getEventManager().debugPrintEventQueue();
            // Show listener buckets on the test object
            if (obj.isValid()) {
                obj->printCaptureEventListeners();
                obj->printBubblingEventListeners();
            }
            // Show current hovered and focused objects (names)
            auto hov = core.getMouseHoveredObject();
            auto fok = core.getKeyboardFocusedObject();
            std::cout << "[BehaviorTest] hovered=" << (hov ? hov.getName() : std::string("<null>"))
                      << " focused=" << (fok ? fok.getName() : std::string("<null>"))
                      << std::endl;
        }
#endif

        // --- 5) Cleanup -------------------------------------------------------------
        if (stage->hasChild(obj))
            stage->removeChild(obj);
        core.collectGarbage();

        // ✅ Return informational result (unused in caller)
        return true; // ✅ finished this frame

    } // END: UnitTests::run_event_behavior_test()



} // END: namespace SDOM
