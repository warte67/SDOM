/**
 * @file main_variant_5.cpp
 * @brief SDOM Startup Variant 5 — verifies the C API main-loop phase helpers.
 */

#include <iostream>
#include <string>

#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/CAPI/SDOM_CAPI_Event.h>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>

#include "Box.hpp"

using namespace SDOM;

namespace
{
    struct Variant5Options
    {
        bool showHelp = false;
        bool verbose = false;
        std::string configPath = "json/config.json";
    };

    void print_variant5_help(const char* exe)
    {
        std::cout
            << "Usage: " << (exe ? exe : "prog") << " --variant 5 [options]\n"
            << "Options:\n"
            << "  --config <file>         Path to JSON config (default: json/config.json)\n"
            << "  --verbose               Print per-step phase verification output\n"
            << "  --help                  Show this help message\n"
            << std::endl;
    }

    Variant5Options parse_options(int argc, char** argv)
    {
        Variant5Options opts;
        for (int i = 1; i < argc; ++i)
        {
            if (!argv[i])
                continue;

            const std::string arg = argv[i];
            if (arg == "--help" || arg == "-?" || arg == "/?")
            {
                opts.showHelp = true;
                break;
            }
            else if ((arg == "--config" || arg == "--json") && i + 1 < argc && argv[i + 1])
            {
                opts.configPath = argv[++i];
            }
            else if (arg == "--verbose")
            {
                opts.verbose = true;
            }
        }
        return opts;
    }

    void register_custom_types(Core& core)
    {
        core.getFactory().registerDisplayObjectType("Box", TypeCreators{
            Box::CreateFromInitStruct,
            Box::CreateFromJson
        });
    }

    struct PhaseVerifier
    {
        explicit PhaseVerifier(bool verboseOutput) : verbose(verboseOutput) {}

        void expect(const std::string& label, bool actual, bool expected, const std::string& detail = {})
        {
            if (actual == expected)
            {
                if (verbose)
                {
                    std::cout << "[Variant 5] PASS: " << label;
                    if (!detail.empty())
                        std::cout << " — " << detail;
                    std::cout << std::endl;
                }
            }
            else
            {
                std::string message = detail;
                if (!message.empty())
                    message += " | ";
                message += "expected ";
                message += expected ? "true" : "false";
                message += ", got ";
                message += actual ? "true" : "false";

                logFailure(label, message);
            }
        }

        void requireEventSnapshot(const std::string& label, const SDOM_Event& evt)
        {
            if (!evt.impl)
                logFailure(label, "expected SDOM_Event snapshot (evt.impl is null)");
            else if (verbose)
                std::cout << "[Variant 5] PASS: " << label << std::endl;
        }

        bool ok() const { return passed; }

    private:
        void logFailure(const std::string& label, const std::string& detail)
        {
            const char* lastError = SDOM_GetError();
            std::cerr << "[Variant 5] FAIL: " << label;
            if (!detail.empty())
                std::cerr << " — " << detail;
            if (lastError && lastError[0])
                std::cerr << " (last error: " << lastError << ")";
            std::cerr << std::endl;
            passed = false;
        }

        bool verbose = false;
        bool passed = true;
    };

    bool drainEventsUntilIdle()
    {
        constexpr int kMaxIterations = 64;
        SDOM_Event evt{};
        for (int i = 0; i < kMaxIterations; ++i)
        {
            const bool hadEvent = SDOM_PollEvents(&evt);
            if (!hadEvent)
            {
                return evt.impl == nullptr;
            }
        }
        return false;
    }

    bool verify_manual_loop_phases(bool verbose)
    {
        PhaseVerifier verifier(verbose);

        verifier.expect("Warm-up SDOM_RunFrame", SDOM_RunFrame(), true, "initialize stage state");

        // Frame A: PollEvents reports empty queue
        bool queueIdle = drainEventsUntilIdle();
        verifier.expect("PollEvents reports empty queue", queueIdle, true);
        verifier.expect("Present after draining events succeeds", SDOM_Present(), true);

        // Frame B: PollEvents returns true when a synthetic event is queued.
        const std::string syntheticMouse = "{\"x\":48,\"y\":32,\"type\":\"down\",\"button\":1,\"clicks\":1}";
        verifier.expect("Inject synthetic mouse event", SDOM_PushMouseEvent(syntheticMouse.c_str()), true);
        SDOM_Event evt{};
        const bool pumped = SDOM_PollEvents(&evt);
        verifier.expect("PollEvents returns true when event pending", pumped, true);
        if (pumped)
            verifier.requireEventSnapshot("PollEvents provides SDOM_Event snapshot", evt);
        verifier.expect("Render before Update heals and reports false", SDOM_Render(), false);
        verifier.expect("Present after auto-heal succeeds", SDOM_Present(), true);

        // Frame C: Proper Poll → Update → Render → Present ordering, plus duplicate Update check.
        (void)drainEventsUntilIdle();

        SDOM_Event idleEvt{};
        const bool idlePoll = SDOM_PollEvents(&idleEvt);
        verifier.expect("Idle PollEvents before Update returns false", idlePoll, false);
        if (!idlePoll)
        {
            const bool snapshotCleared = (idleEvt.impl == nullptr);
            verifier.expect("Idle PollEvents snapshot cleared", snapshotCleared, true);
        }

        verifier.expect("Update after PollEvents succeeds", SDOM_Update(), true);
        verifier.expect("Second Update call returns false", SDOM_Update(), false);
        verifier.expect("Render after valid Update succeeds", SDOM_Render(), true);
        verifier.expect("Present after full sequence succeeds", SDOM_Present(), true);

        // Frame D: Present should auto-run prerequisites and still report success.
        verifier.expect("Standalone Present auto-heals and succeeds", SDOM_Present(), true);

        // Frame E: RunFrame convenience helper should succeed unless fatal errors occur.
        verifier.expect("SDOM_RunFrame executes cleanly", SDOM_RunFrame(), true);

        return verifier.ok();
    }
}

int SDOM_main_variant_5(int argc, char** argv)
{
    Variant5Options opts = parse_options(argc, argv);
    if (opts.showHelp)
    {
        print_variant5_help(argv ? argv[0] : nullptr);
        return 0;
    }

    if (!SDOM_Init(0))
    {
        const char* apiError = SDOM_GetError();
        std::cerr << "[Variant 5] ERROR: SDOM_Init failed: "
                  << (apiError ? apiError : "(no error message)")
                  << std::endl;
        return 1;
    }

    bool initialized = true;
    auto shutdown = [&]() {
        if (initialized)
        {
            SDOM_Quit();
            initialized = false;
        }
    };

    Core& core = Core::getInstance();
    register_custom_types(core);

    if (!SDOM_SetIgnoreRealInput(true))
    {
        std::cerr << "[Variant 5] WARNING: unable to enable ignore-real-input filter" << std::endl;
    }

    const bool loaded = SDOM_LoadDomFromJsonFile(opts.configPath.c_str());
    if (!loaded)
    {
        const char* apiError = SDOM_GetError();
        std::cerr << "[Variant 5] ERROR: SDOM_LoadDomFromJsonFile('" << opts.configPath
                  << "') failed: " << (apiError ? apiError : "(no error message)") << std::endl;
        shutdown();
        return 1;
    }

    const bool phasesOK = verify_manual_loop_phases(opts.verbose);

    if (!SDOM_SetIgnoreRealInput(false))
    {
        std::cerr << "[Variant 5] WARNING: unable to restore real input filtering state" << std::endl;
    }

    if (phasesOK)
    {
        std::cout << "[Variant 5] C API main-loop verification succeeded." << std::endl;
    }
    else
    {
        std::cerr << "[Variant 5] C API main-loop verification FAILED." << std::endl;
    }

    shutdown();
    return phasesOK ? 0 : 1;
}
