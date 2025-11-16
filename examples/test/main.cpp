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
#include <SDOM/SDOM_Utils.hpp>

#include <SDOM/SDOM_TruetypeFont.hpp>
#include <SDOM/SDOM_BitmapFont.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>

#include <iostream>
#include <cassert>


#include "Box.hpp"
#include "UnitTests.hpp"


#ifndef MAIN_VARIANT
#define MAIN_VARIANT 2
#endif


using namespace SDOM;

#if MAIN_VARIANT == 1

int main(int argc, char** argv) 
{
    // std::cout << CLR::ORANGE << CLR::ARROW_UP << CLR::NORMAL << std::endl;

    // Fetch the Core singleton
    Core& core = getCore();

    // SDOM::Core core;
    // SDOM::setCore(&core);  // see implementation below        

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
    core.getFactory().registerDisplayObjectType("Box", TypeCreators{
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

    // fresh tests
    lua["Core_UnitTests"] = &SDOM::Core_UnitTests;
    lua["Variant_UnitTests"] = &SDOM::Variant_UnitTests;
    lua["DataRegistry_UnitTests"] = &SDOM::DataRegistry_UnitTests;
    lua["Event_CAPI_UnitTests"] = &SDOM::Event_CAPI_UnitTests;
    lua["EventType_CAPI_UnitTests"] = &SDOM::EventType_CAPI_UnitTests;

    // stale legacy tests
    lua["IDisplayObject_UnitTests"] = &SDOM::IDisplayObject_UnitTests;
    lua["Event_UnitTests"] = &SDOM::Event_UnitTests;
    lua["EventType_UnitTests"] = &SDOM::EventType_UnitTests;
    lua["ArrowButton_UnitTests"] = &SDOM::ArrowButton_UnitTests;
    lua["AssetHandle_UnitTests"] = &SDOM::AssetHandle_UnitTests;
    lua["BitmapFont_UnitTests"] = &SDOM::BitmapFont_UnitTests;
    lua["Button_UnitTests"] = &SDOM::Button_UnitTests;
    lua["TristateButton_UnitTests"] = &SDOM::TristateButton_UnitTests;


    
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



// --- Main UnitTests Runner --- //
bool Main_UnitTests()
{
    bool done = true;

    done &= IDisplayObject_UnitTests();
    done &= DataRegistry_UnitTests();
    done &= Event_CAPI_UnitTests();
    done &= EventType_CAPI_UnitTests();    
    done &= Variant_UnitTests();    

    return done;
} // END: EventType_UnitTests()



int main(int argc, char** argv) 
{
    // headless / no-lua startup for fast unit testing
    SDOM::Core& core = getCore();
    Factory& factory = core.getFactory();

    // Configure Core with windows size, pixel size, etc.
    SDOM::Core::CoreConfig cfg;
    cfg.windowWidth = 1200.0f;
    cfg.windowHeight = 800.0f;
    cfg.pixelWidth = 2.0f;
    cfg.pixelHeight = 2.0f;
    cfg.allowTextureResize = false;
    cfg.preserveAspectRatio = true;
    cfg.rendererLogicalPresentation = SDL_LOGICAL_PRESENTATION_LETTERBOX;
    cfg.windowFlags = SDL_WINDOW_RESIZABLE;
    cfg.pixelFormat = SDL_PIXELFORMAT_RGBA8888;
    cfg.color = SDL_Color{8, 0, 16, 255};
    core.configure(cfg);

    // register the Main_UnitTests function to be called during the unit test phase
    core.registerOnUnitTest(Main_UnitTests);

    TruetypeFont::InitStruct varela16;
    varela16.name = "VarelaRound16";
    varela16.type = "TruetypeFont";
    varela16.filename = "./assets/VarelaRound.ttf";
    varela16.font_size = 16;
    AssetHandle varela16_handle = factory.createAssetObject("TruetypeFont", varela16);

    TruetypeFont::InitStruct varela32;
    varela32.name = "VarelaRound32";
    varela32.type = "TruetypeFont";
    varela32.filename = "./assets/VarelaRound.ttf";
    varela32.font_size = 32;
    AssetHandle varela32_handle = factory.createAssetObject("TruetypeFont", varela32);

    BitmapFont::InitStruct external_font_8x8;
    external_font_8x8.name = "external_font_8x8";
    external_font_8x8.type = "BitmapFont";
    external_font_8x8.filename = "./assets/font_8x8.png";
    external_font_8x8.font_width = 8;
    external_font_8x8.font_height = 8;
    AssetHandle externalFont8x8_handle = factory.createAssetObject("BitmapFont", external_font_8x8);

    BitmapFont::InitStruct external_font_8x12;
    external_font_8x12.name = "external_font_8x12";
    external_font_8x12.type = "BitmapFont";
    external_font_8x12.filename = "./assets/font_8x12.png";
    external_font_8x12.font_width = 8;
    external_font_8x12.font_height = 12;
    AssetHandle externalFont8x12_handle = factory.createAssetObject("BitmapFont", external_font_8x12);

    SpriteSheet::InitStruct external_icon_8x8;
    external_icon_8x8.name = "external_icon_8x8";
    external_icon_8x8.type = "SpriteSheet";
    external_icon_8x8.filename = "./assets/icon_8x8.png";
    external_icon_8x8.spriteWidth = 8;
    external_icon_8x8.spriteHeight = 8;
    AssetHandle externalIcon8x8_handle = factory.createAssetObject("SpriteSheet", external_icon_8x8);

    // Register minimal types needed by tests
    core.getFactory().registerDisplayObjectType("Box", TypeCreators{
        Box::CreateFromInitStruct,
        Box::CreateFromJson
    });

    // Now that the Factory is initialized, create the Stage and set it as root
    SDOM::Stage::InitStruct stage_init;
    stage_init.name = "mainStage";
    stage_init.type = "Stage";
    stage_init.color = SDL_Color{16, 32, 8, 255};
    DisplayHandle rootStage = core.createDisplayObject("Stage", stage_init);
    core.setRootNode(rootStage);

    // Create the right frame
    DisplayHandle rightMainFrame = factory.createDisplayObjectFromJson("Frame",
        nlohmann::json{
            {"name", "rightMainFrame"},
            {"type", "Frame"},
            {"text", "This should not be a Label."},
            {"x", 300},
            {"y", 5},
            {"width", 295},
            {"height", 390},
            {"icon_resource", "internal_icon_16x16"},
            {"color",
                {
                    {"r", 32}, {"g", 64}, {"b", 16}, {"a", 255}
                }
            }
        }
    );
    rootStage->addChild(rightMainFrame);

    // Main Group 
    DisplayHandle mainGroup = factory.createDisplayObjectFromJson(
        "Group",
        nlohmann::json{
            {"name", "mainFrameGroup"},
            {"type", "Group"},
            {"x", 450},
            {"y", 10},
            {"width", 140},
            {"height", 70},
            {"text", "Main Group"},
            {"icon_resource", "internal_icon_16x16"},
            {"font_resource", "internal_ttf"},
            {"font_size", 12},
            {"color", {
                {"r", 255}, {"g", 255}, {"b", 255}, {"a", 96}
            }},
            {"label_color", {
                {"r", 224}, {"g", 192}, {"b", 192}, {"a", 255}
            }}
        }
    );
    rightMainFrame->addChild(mainGroup);

    rootStage->printTree();


    // Start the main loop (no additional config needed)
    // core.setStopAfterUnitTests(true);
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