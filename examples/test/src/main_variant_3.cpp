/**
 * @file main_variant_3.cpp
 * @brief SDOM Startup Variant 3 — C API driven bootstrap mirroring variant 4.
 */

#include <iostream>
#include <string>

#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>

#include "Box.hpp"
#include "UnitTests.hpp"

using namespace SDOM;

namespace
{
    struct Variant3Options
    {
        bool stopAfterTests = false;
        bool showHelp = false;
        std::string configPath = "json/config.json";
    };

    void print_variant3_help(const char* exe)
    {
        std::cout
            << "Usage: " << (exe ? exe : "prog") << " --variant 3 [options]\n"
            << "Options:\n"
            << "  --config <file>         Path to JSON config (default: json/config.json)\n"
            << "  --stop_after_tests      Run unit tests then exit\n"
            << "  --stop-after-tests      Alias for --stop_after_tests\n"
            << "  --help                  Show this help message\n"
            << std::endl;
    }

    Variant3Options parse_options(int argc, char** argv)
    {
        Variant3Options opts;
        for (int i = 1; i < argc; ++i)
        {
            if (!argv[i]) continue;
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
            else if (arg == "--stop_after_tests" || arg == "--stop-after-tests")
            {
                opts.stopAfterTests = true;
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
}

extern bool Main_UnitTests();

/* ---------------------------------------------------------------------------
   Variant 3 entry point (C API mode)
--------------------------------------------------------------------------- */
int SDOM_main_variant_3(int argc, char** argv)
{
    Variant3Options opts = parse_options(argc, argv);
    if (opts.showHelp)
    {
        print_variant3_help(argv ? argv[0] : nullptr);
        return 0;
    }

    if (!SDOM_Init(0))
    {
        const char* apiError = SDOM_GetError();
        std::cerr << "[Variant 3] ERROR: SDOM_Init failed: "
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
    core.registerOnUnitTest(Main_UnitTests);
    register_custom_types(core);

    if (!SDOM_SetStopAfterUnitTests(opts.stopAfterTests))
    {
        const char* apiError = SDOM_GetError();
        std::cerr << "[Variant 3] ERROR: SDOM_SetStopAfterUnitTests failed: "
                  << (apiError ? apiError : "(no error message)")
                  << std::endl;
        shutdown();
        return 1;
    }

    if (!SDOM_LoadDomFromJsonFile(opts.configPath.c_str()))
    {
        const char* apiError = SDOM_GetError();
        std::cerr << "[Variant 3] ERROR: SDOM_LoadDomFromJsonFile('" << opts.configPath
                  << "') failed: " << (apiError ? apiError : "(no error message)") << std::endl;
        shutdown();
        return 1;
    }

    if (!SDOM_Run())
    {
        const char* apiError = SDOM_GetError();
        std::cerr << "[Variant 3] ERROR: SDOM_Run reported failure: "
                  << (apiError ? apiError : "(no error message)")
                  << std::endl;
        shutdown();
        return 1;
    }

    shutdown();
    return 0;
}
