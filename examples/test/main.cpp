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

#define SDOM_USE_INDIVIDUAL_HEADERS
#include <SDOM/SDOM.hpp>

#ifdef SDOM_USE_INDIVIDUAL_HEADERS
    #include <SDOM/SDOM_Core.hpp>
    #include <SDOM/SDOM_Factory.hpp>
    #include <SDOM/SDOM_Event.hpp>
    #include <SDOM/SDOM_IUnitTest.hpp>
    #include <SDOM/SDOM_IDataObject.hpp>
#endif

#include "Box.hpp"
#include "UnitTests.hpp"

using namespace SDOM;


int main(int argc, char** argv) 
{
    std::cout << CLR::ORANGE << CLR::ARROW_UP << CLR::NORMAL << std::endl;

    // Quick command-line help handling: if user asks for help, print usage
    // and exit without initializing the full Core.
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) continue;
        std::string a(argv[i]);
        if (a == "--help" || a == "-?" || a == "/?") {
            std::cout << "Usage: " << (argv[0] ? argv[0] : "prog") << " [options]\n"
                      << "Options:\n"
                      << "  --lua_file <file>     Specify a Lua config file to run (overrides default).\n"
                      << "                       Aliases: --file, --config, --lua\n"
                      << "  --stop_after_tests    Run unit tests then stop the main loop and exit.\n"
                      << "                       Aliases: --stop-after-tests\n"
                      << "  --help, -?, /?        Show this help message and exit.\n"
                      << std::endl;
            return 0;
        }
    }

    // std::cout << CLR::YELLOW << "Hello, SDOM!" << CLR::RESET << std::endl;

    // Fetch the Core singleton
    Core& core = getCore();


    // For testing: optionally stop main loop after unit tests if caller
    // requested it via command-line flag `--stop_after_tests`.
    bool stopAfterTests = false;
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) continue;
        std::string s(argv[i]);
        if (s == "--stop_after_tests" || s == "--stop-after-tests") {
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

    // Expose C++ unit test functions to Lua so scripts can call them to run tests
    // (these are defined in UnitTests.hpp)

    lua["Core_UnitTests"] = &SDOM::Core_UnitTests;
    lua["IDisplayObject_UnitTests"] = &SDOM::IDisplayObject_UnitTests;
    
    lua["Factory_UnitTests"] = &SDOM::Factory_UnitTests;
    lua["Stage_UnitTests"] = &SDOM::Stage_UnitTests;
    lua["Box_UnitTests"] = &SDOM::Box_UnitTests;
    lua["LUA_UnitTests"] = &SDOM::LUA_UnitTests;
    lua["DisplayHandle_UnitTests"] = &SDOM::DisplayHandle_UnitTests;
    lua["EventType_UnitTests"] = &SDOM::EventType_UnitTests;
    lua["Event_UnitTests"] = &SDOM::Event_UnitTests;
    lua["GarbageCollection_UnitTests"] = &SDOM::GarbageCollection_UnitTests;
    lua["SpriteSheet_UnitTests"] = &SDOM::SpriteSheet_UnitTests;
    lua["Label_UnitTests"] = &SDOM::Label_UnitTests;
    lua["Group_UnitTests"] = &SDOM::Group_UnitTests;
    
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