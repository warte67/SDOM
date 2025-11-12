/**
 * @file main.cpp
 * @author Jay Faries (https://github.com/warte67/SDOM)
 * @brief Main entry point for the SDOM example test application.

 * @date 2025-09-19
 * 
 * @copyright Copyright (c) 2025
 * 
 * 
 * 
 * Note:  sudo apt install liblua5.3-dev  or  sudo apt install liblua5.4-dev
 * 
//  */

// #define SOL_ALL_AUTOMAGIC 0
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
 
// For development, define SDOM_USE_INDIVIDUAL_HEADERS and include only 
// the SDOM headers you need. This speeds up incremental builds and makes 
// dependencies explicit.

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_CAPI.h>

#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_IUnitTest.hpp>
#include <SDOM/SDOM_IDataObject.hpp>
#include <SDOM/SDOM_Stage.hpp>

#include <iostream>



#include "Box.hpp"
#include "UnitTests.hpp"


#ifndef MAIN_VARIANT
#define MAIN_VARIANT 3
#endif


using namespace SDOM;

#if MAIN_VARIANT == 1

int main(int argc, char** argv) 
{
    // std::cout << CLR::ORANGE << CLR::ARROW_UP << CLR::NORMAL << std::endl;

    // Fetch the Core singleton
    Core& core = getCore();

    // -------------------------------------------------------
    // Quick command-line help handling
    // -------------------------------------------------------
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) continue;
        std::string a(argv[i]);
        if (a == "--help" || a == "-?" || a == "/?") {
            std::cout << "Usage: " << (argv[0] ? argv[0] : "prog") << " [options]\n"
                      << "Options:\n"
                      << "  --version             Show the SDOM version and build info.\n"
                      << "  --lua_file <file>     Specify a Lua config file to run (overrides default).\n"
                      << "                        Aliases: --file, --config, --lua\n"
                      << "  --stop_after_tests    Run unit tests then stop the main loop and exit.\n"
                      << "                        Aliases: --stop-after-tests\n"
                      << "  --show_performance    Show per-object performance stats after each frame.\n"
                      << "                        Aliases: --show-performance\n"
                      << "  --help, -?, /?        Show this help message and exit.\n"
                      << std::endl;
            return 0;
        }
        // -------------------------------------------------------
        // Handle version flag before initializing Core
        // -------------------------------------------------------
        else if (a == "--version" || a == "-v") {
            std::cout << "SDOM " 
                      << core.getVersionMajor() << "."
                      << core.getVersionMinor() << "."
                      << core.getVersionPatch()
                      << " (" << core.getVersionCodename() << ")\n"
                      << "Build Date: " << core.getVersionBuildDate() << "\n"
                      << "Commit: " << core.getVersionCommit() << "\n"
                      << "Branch: " << core.getVersionBranch() << "\n"
                      << "Compiler: " << core.getVersionCompiler() << "\n"
                      << "Platform: " << core.getVersionPlatform() << "\n"
                      << std::endl;
            return 0;
        }
    }

    // -------------------------------------------------------
    // Normal initialization continues...
    // -------------------------------------------------------

    // For testing: optionally stop main loop after unit tests if caller
    // requested it via command-line flag `--stop_after_tests`.
    bool stopAfterTests = false;
    for (int i = 1; i < argc; ++i) 
    {
        if (!argv[i]) continue;
        std::string s(argv[i]);
        if (s == "--stop_after_tests" || s == "--stop-after-tests") 
        {
            stopAfterTests = true;
            break;
        }
    }
    core.setStopAfterUnitTests(stopAfterTests);

    // register any custom DisplayHandle types before configuring the Core
    core.getFactory().registerDomType("Box", TypeCreators{
        Box::CreateFromLua,
        Box::CreateFromInitStruct
    });    

    // Use Core's Lua state
    sol::state& lua = core.getLua();     

    // Ensure Lua-driven unit tests run in the default (variant 1) startup.
    // This flag is read by `examples/test/lua/callbacks/unittest.lua` and
    // allows C++ variants to disable Lua test execution when needed.
    lua["SDOM_RUN_LUA_UNIT_TESTS"] = true;

    // Expose C++ unit test functions to Lua so scripts can call them to run tests
    // (these are defined in UnitTests.hpp)

    lua["Core_UnitTests"] = &SDOM::Core_UnitTests;
    lua["IDisplayObject_UnitTests"] = &SDOM::IDisplayObject_UnitTests;
    lua["Event_UnitTests"] = &SDOM::Event_UnitTests;
    lua["EventType_UnitTests"] = &SDOM::EventType_UnitTests;
    lua["ArrowButton_UnitTests"] = &SDOM::ArrowButton_UnitTests;
    lua["AssetHandle_UnitTests"] = &SDOM::AssetHandle_UnitTests;
    lua["BitmapFont_UnitTests"] = &SDOM::BitmapFont_UnitTests;
    lua["Button_UnitTests"] = &SDOM::Button_UnitTests;

    lua["TristateButton_UnitTests"] = &SDOM::TristateButton_UnitTests;

    lua["Variant_UnitTests"] = &SDOM::Variant_UnitTests;
    lua["DataRegistry_UnitTests"] = &SDOM::DataRegistry_UnitTests;
    
    // Allow overriding which Lua config to run via command-line.
    // Priority:
    //  1) --file <file> (preferred)
    //  1b) --config <file> (alias for backward compatibility)
    //  2) first positional non-flag argument (not starting with - or /)
    //  3) default: "lua/config.lua"
    std::string configFile = "lua/config.lua";
    // Check for --lua_file <file> or aliases (--file, --config, --lua)
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) continue;
        std::string a(argv[i]);
        if ((a == "--lua_file" || a == "--file" || a == "--config" || a == "--lua") && (i + 1) < argc && argv[i + 1]) {
            configFile = std::string(argv[i + 1]);
            break;
        }
    }
    // If no --config provided, look for first positional argument
    if (configFile == "lua/config.lua") {
        for (int i = 1; i < argc; ++i) {
            if (!argv[i]) continue;
            std::string a(argv[i]);
            // skip known flags
            if (a.rfind("--", 0) == 0) continue;
            if (!a.empty() && a[0] == '-') continue;
            if (a == "/?" || a == "-?") continue;
            // skip help and our known flags/aliases
            if (a == "--help" || a == "--stop_after_tests" || a == "--stop-after-tests" || a == "--config" || a == "--file" || a == "--lua_file" || a == "--lua") continue;
            // treat as config filename
            configFile = a;
            break;
        }
    }

    // std::cout << "[INFO] Using Lua config: " << configFile << std::endl;
    // Configure the Core from the selected Lua config file
    bool ok = core.run(configFile);
    return ok ? 0 : 1;
} // END main()

#elif MAIN_VARIANT == 2

int main(int argc, char** argv) 
{
    // headless / no-lua startup for fast unit testing
    Core& core = getCore();

    // Apply an explicit Core configuration first so SDL resources and
    // the Factory are initialized before we create the Stage object.
    SDOM::Core::CoreConfig cfg; // defaults are reasonable (window + renderer will be created)
    core.configure(cfg);

    // Disable Lua-driven unit tests for this headless variant
    core.getLua()["SDOM_RUN_LUA_UNIT_TESTS"] = false;

    // Register minimal types needed by tests
    core.getFactory().registerDomType("Box", TypeCreators{
        Box::CreateFromLua,
        Box::CreateFromInitStruct
    });

    // Now that the Factory is initialized, create the Stage and set it as root
    SDOM::Stage::InitStruct stage_init;

    DisplayHandle rootStage = core.createDisplayObject("Stage", stage_init);
    core.setRootNode(rootStage);

    // Start the main loop (no additional config needed)
    core.setStopAfterUnitTests(true);
    bool ok = core.run();
    
    return ok ? 0 : 1;
}

#elif MAIN_VARIANT == 3

int main(int argc, char** argv) 
{
    SDOM_Init();

    SDOM_CoreConfig cfg;
    SDOM_GetCoreConfig(&cfg);
    cfg.windowWidth = 1280.0f;
    cfg.windowHeight = 960.0f;
    SDOM_Configure(&cfg);


    SDOM_Quit();
    
    return 0;
}

#else
int main(int argc, char** argv) 
{
    std::cerr << "Unknown MAIN_VARIANT: " << MAIN_VARIANT << std::endl;
    return 1;
}
#endif