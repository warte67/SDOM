/**
 * @file main_variant_1.cpp
 * @brief SDOM Startup Variant 1 — Lua-driven frontend configuration.
 *
 * This variant mirrors the original behavior of MAIN_VARIANT == 1:
 *   - Full Lua environment
 *   - Lua config file drives object creation
 *   - Lua-driven unit tests available
 */

#include <iostream>
#include <string>
#include <sol/sol.hpp>

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_IUnitTest.hpp>

#include "Box.hpp"
#include "UnitTests.hpp"

using namespace SDOM;

/* ---------------------------------------------------------------------------
   Helper: Print command line usage
--------------------------------------------------------------------------- */
static void print_help(const char* progName)
{
    std::cout
        << "Usage: " << (progName ? progName : "sdom_app") << " [options]\n"
        << "Options:\n"
        << "  --version               Show the SDOM version and build info.\n"
        << "  --lua_file <file>       Run Lua config file (default: lua/config.lua)\n"
        << "     Aliases: --file, --config, --lua\n"
        << "  --stop_after_tests      Run unit tests then exit.\n"
        << "  --show_performance      Show per-object performance each frame.\n"
        << "  --help, -?, /?          Show this help message.\n\n";
}

/* ---------------------------------------------------------------------------
   Helper: Print SDOM version info
--------------------------------------------------------------------------- */
static void print_version(Core& core)
{
    std::cout
        << "SDOM "
        << core.getVersionMajor() << "."
        << core.getVersionMinor() << "."
        << core.getVersionPatch()
        << " (" << core.getVersionCodename() << ")\n"
        << "Build Date: "      << core.getVersionBuildDate() << "\n"
        << "Commit: "          << core.getVersionCommit() << "\n"
        << "Branch: "          << core.getVersionBranch() << "\n"
        << "Compiler: "        << core.getVersionCompiler() << "\n"
        << "Platform: "        << core.getVersionPlatform() << "\n\n";
}

/* ---------------------------------------------------------------------------
   Variant 1 entry point
--------------------------------------------------------------------------- */
int SDOM_main_variant_1(int argc, char** argv)
{
    Core& core = getCore();

    // -----------------------------------------
    // Parse simple command-line flags first
    // -----------------------------------------
    bool stopAfterTests = false;
    std::string configFile = "lua/config.lua";

    for (int i = 1; i < argc; ++i)
    {
        if (!argv[i]) continue;
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-?" || arg == "/?")
        {
            print_help(argv[0]);
            return 0;
        }
        else if (arg == "--version" || arg == "-v")
        {
            print_version(core);
            return 0;
        }
        else if (arg == "--stop_after_tests" || arg == "--stop-after-tests")
        {
            stopAfterTests = true;
        }
        else if ((arg == "--lua_file" || arg == "--file" ||
                  arg == "--config"   || arg == "--lua")
                 && i + 1 < argc && argv[i + 1])
        {
            configFile = argv[++i];
        }
    }

    // -----------------------------------------
    // If config file not overridden, check for
    // first positional non-flag argument
    // -----------------------------------------
    if (configFile == "lua/config.lua")
    {
        for (int i = 1; i < argc; ++i)
        {
            std::string a(argv[i]);
            if (a.empty()) continue;

            // skip flags
            if (a[0] == '-') continue;
            if (a == "/?" || a == "-?") continue;

            // treat as Lua config filename
            configFile = a;
            break;
        }
    }

    // -----------------------------------------
    // Apply stop-after-tests flag
    // -----------------------------------------
    core.setStopAfterUnitTests(stopAfterTests);

    // // -----------------------------------------
    // // Register custom types BEFORE Lua config
    // // -----------------------------------------
    // core.getFactory().registerDisplayObjectType(
    //     "Box",
    //     TypeCreators{
    //         Box::CreateFromLua,
    //         Box::CreateFromInitStruct
    //     }
    // );

    // // -----------------------------------------
    // // Prepare Lua state
    // // -----------------------------------------
    // sol::state& lua = core.getLua();

    // lua["SDOM_RUN_LUA_UNIT_TESTS"] = true;

    // // Expose C++ unit tests to Lua test harness
    // lua["Core_UnitTests"]       = &SDOM::Core_UnitTests;
    // lua["Variant_UnitTests"]    = &SDOM::Variant_UnitTests;
    // lua["DataRegistry_UnitTests"] = &SDOM::DataRegistry_UnitTests;
    // lua["Event_CAPI_UnitTests"] = &SDOM::Event_CAPI_UnitTests;
    // lua["EventType_CAPI_UnitTests"] = &SDOM::EventType_CAPI_UnitTests;

    // // Legacy tests
    // lua["IDisplayObject_UnitTests"] = &SDOM::IDisplayObject_UnitTests;
    // lua["Event_UnitTests"]          = &SDOM::Event_UnitTests;
    // lua["EventType_UnitTests"]      = &SDOM::EventType_UnitTests;
    // lua["ArrowButton_UnitTests"]    = &SDOM::ArrowButton_UnitTests;
    // lua["AssetHandle_UnitTests"]    = &SDOM::AssetHandle_UnitTests;
    // lua["BitmapFont_UnitTests"]     = &SDOM::BitmapFont_UnitTests;
    // lua["Button_UnitTests"]         = &SDOM::Button_UnitTests;
    // lua["TristateButton_UnitTests"] = &SDOM::TristateButton_UnitTests;

    // // -----------------------------------------
    // // Execute the Lua-driven config file
    // // -----------------------------------------
    // bool ok = core.run(configFile);
    bool ok = true;

    return ok ? 0 : 1;
}
