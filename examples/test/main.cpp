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
    done &= FrontEnd_UnitTests();

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
    stage_init.color = SDL_Color{32, 8, 4, 255};
    DisplayHandle rootStage = core.createDisplayObject("Stage", stage_init);
    core.setRootNode(rootStage);

    // Create the right frame
    DisplayHandle rightMainFrame = factory.createDisplayObjectFromJson("Frame",
        nlohmann::json{
            {"name", "rightMainFrame"},
            {"type", "Frame"},
            {"text", "Hello World."},
            {"x", 300},
            {"y", 5},
            {"width", 295},
            {"height", 390},
            {"icon_resource", "internal_icon_16x16"},
            {"color",
                {
                    {"r", 64}, {"g", 32}, {"b", 32}, {"a", 255}
                }
            }
        }
    );
    rootStage->addChild(rightMainFrame);


    // === Main Frame Group ======================================================
    DisplayHandle mainGroup =
        factory.createDisplayObjectFromJson(
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
                {"color",     { {"r",224}, {"g",192}, {"b",192}, {"a",192} }},
                {"label_color",{ {"r",224}, {"g",192}, {"b",192}, {"a",255} }}
            }
        );
    rightMainFrame->addChild(mainGroup);


    // === RadioButton #1 =========================================================
    DisplayHandle rb1 =
        factory.createDisplayObjectFromJson(
            "RadioButton",
            nlohmann::json{
                {"name", "mainFrameGroup_radioButton_1"},
                {"type", "RadioButton"},
                {"x", 460},
                {"y", 24},
                {"width", 120},
                {"height", 16},
                {"color",       { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"label_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"font_resource", "internal_ttf"},
                {"icon_resource", "internal_icon_16x16"},
                {"font_size", 10},
                {"selected", true},
                {"text", "16x16 internal_ttf"}
            }
        );
    mainGroup->addChild(rb1);


    // === RadioButton #2 =========================================================
    DisplayHandle rb2 =
        factory.createDisplayObjectFromJson(
            "RadioButton",
            nlohmann::json{
                {"name", "mainFrameGroup_radioButton_2"},
                {"type", "RadioButton"},
                {"x", 460},
                {"y", 40},
                {"width", 120},
                {"height", 16},
                {"color",       { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"label_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"font_resource", "VarelaRound16"},
                {"icon_resource", "internal_icon_12x12"},
                {"font_size", 10},
                {"text", "12x12 Varela TTF"},
                {"checked", true}  // alias for selected (supported by JSON loader)
            }
        );
    mainGroup->addChild(rb2);


    // === RadioButton #3 =========================================================
    DisplayHandle rb3 =
        factory.createDisplayObjectFromJson(
            "RadioButton",
            nlohmann::json{
                {"name", "mainFrameGroup_radioButton_3"},
                {"type", "RadioButton"},
                {"x", 460},
                {"y", 56},
                {"width", 120},
                {"height", 16},
                {"color",       { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"label_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"icon_resource", "internal_icon_8x8"},
                {"text", "8x8 bmp font"}
            }
        );
    mainGroup->addChild(rb3);



    // === CheckButton Set ========================================================
    DisplayHandle cb1 =
        factory.createDisplayObjectFromJson(
            "CheckButton",
            nlohmann::json{
                {"name", "mainFrame_CheckButton_1"},
                {"type", "CheckButton"},
                {"x", 305},
                {"y", 10},
                {"width", 145},
                {"height", 16},
                {"color",       { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"label_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"icon_resource", "internal_icon_12x12"},
                {"font_resource", "internal_ttf"},
                {"font_size", 10},
                {"text", "internal_ttf CheckButton"}
            }
        );
    rightMainFrame->addChild(cb1);

    DisplayHandle cb2 =
        factory.createDisplayObjectFromJson(
            "CheckButton",
            nlohmann::json{
                {"name", "mainFrame_CheckButton_2"},
                {"type", "CheckButton"},
                {"x", 305},
                {"y", 26},
                {"width", 145},
                {"height", 16},
                {"color",       { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"label_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"icon_resource", "internal_icon_12x12"},
                {"font_resource", "VarelaRound16"},
                {"font_size", 10},
                {"text", "VarelaRound16 CheckButton"}
            }
        );
    rightMainFrame->addChild(cb2);

    DisplayHandle cb3 =
        factory.createDisplayObjectFromJson(
            "CheckButton",
            nlohmann::json{
                {"name", "mainFrame_CheckButton_3"},
                {"type", "CheckButton"},
                {"x", 305},
                {"y", 42},
                {"width", 145},
                {"height", 16},
                {"color",       { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"label_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"icon_resource", "internal_icon_12x12"},
                {"text", "font_8x8 CheckButton"}
            }
        );
    rightMainFrame->addChild(cb3);


    // === Additional RadioButtons on Frame ======================================
    DisplayHandle frameRb1 =
        factory.createDisplayObjectFromJson(
            "RadioButton",
            nlohmann::json{
                {"name", "mainFrame_RadioButton_1"},
                {"type", "RadioButton"},
                {"x", 305},
                {"y", 56},
                {"width", 145},
                {"height", 16},
                {"color",       { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"label_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"icon_resource", "internal_icon_12x12"},
                {"font_resource", "internal_ttf"},
                {"font_size", 10},
                {"text", "internal_ttf RadioButton"},
                {"selected", true}
            }
        );
    rightMainFrame->addChild(frameRb1);

    DisplayHandle frameRb2 =
        factory.createDisplayObjectFromJson(
            "RadioButton",
            nlohmann::json{
                {"name", "mainFrame_RadioButton_2"},
                {"type", "RadioButton"},
                {"x", 305},
                {"y", 72},
                {"width", 145},
                {"height", 16},
                {"color",       { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"label_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"icon_resource", "internal_icon_12x12"},
                {"font_resource", "VarelaRound16"},
                {"font_size", 10},
                {"text", "VarelaRound16 RadioButton"}
            }
        );
    rightMainFrame->addChild(frameRb2);

    DisplayHandle frameRb3 =
        factory.createDisplayObjectFromJson(
            "RadioButton",
            nlohmann::json{
                {"name", "mainFrame_RadioButton_3"},
                {"type", "RadioButton"},
                {"x", 305},
                {"y", 88},
                {"width", 145},
                {"height", 16},
                {"color",       { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"label_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"icon_resource", "internal_icon_12x12"},
                {"text", "font_8x8 RadioButton"}
            }
        );
    rightMainFrame->addChild(frameRb3);


    // === Tristate Button ========================================================
    DisplayHandle tristate =
        factory.createDisplayObjectFromJson(
            "TristateButton",
            nlohmann::json{
                {"name", "mainFrame_tristate_1"},
                {"type", "TristateButton"},
                {"x", 305},
                {"y", 104},
                {"width", 145},
                {"height", 16},
                {"color",       { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"label_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"icon_resource", "internal_icon_12x12"},
                {"font_resource", "VarelaRound16"},
                {"font_size", 10},
                {"text", "VarelaRound16 TristateButton"}
            }
        );
    rightMainFrame->addChild(tristate);


    // === Horizontal Slider / Progress / Scroll ================================
    DisplayHandle hSlider =
        factory.createDisplayObjectFromJson(
            "Slider",
            nlohmann::json{
                {"name", "mainFrame_hslider_1"},
                {"type", "Slider"},
                {"orientation", "horizontal"},
                {"x", 450},
                {"y", 84},
                {"width", 140},
                {"height", 16},
                {"icon_resource", "internal_icon_16x16"},
                {"step", 2.5},
                {"background_color", { {"r",16}, {"g",16}, {"b",16}, {"a",0} }},
                {"border_color",     { {"r",0},  {"g",0},  {"b",0},  {"a",0} }}
            }
        );
    rightMainFrame->addChild(hSlider);

    DisplayHandle hProgress =
        factory.createDisplayObjectFromJson(
            "ProgressBar",
            nlohmann::json{
                {"name", "mainFrame_hprogress_1"},
                {"type", "ProgressBar"},
                {"orientation", "horizontal"},
                {"x", 450},
                {"y", 104},
                {"width", 140},
                {"height", 16},
                {"icon_resource", "internal_icon_16x16"},
                {"value", 50.0},
                {"step", 2.5},
                {"color", { {"r",255}, {"g",0}, {"b",0}, {"a",255} }},
                {"background_color", { {"r",16}, {"g",16}, {"b",16}, {"a",0} }}
            }
        );
    rightMainFrame->addChild(hProgress);

    DisplayHandle hScroll =
        factory.createDisplayObjectFromJson(
            "ScrollBar",
            nlohmann::json{
                {"name", "mainFrame_hscrollbar_1"},
                {"type", "ScrollBar"},
                {"orientation", "horizontal"},
                {"x", 450},
                {"y", 128},
                {"width", 140},
                {"height", 16},
                {"icon_resource", "internal_icon_16x16"},
                {"step", 5.0},
                {"alignment", "left"},
                {"border_color", { {"r",0}, {"g",0}, {"b",0}, {"a",128} }}
            }
        );
    rightMainFrame->addChild(hScroll);


    // === Vertical Slider / Progress / Scroll ==================================
    DisplayHandle vSlider =
        factory.createDisplayObjectFromJson(
            "Slider",
            nlohmann::json{
                {"name", "mainFrame_vslider_1"},
                {"type", "Slider"},
                {"orientation", "vertical"},
                {"x", 574},
                {"y", 154},
                {"width", 16},
                {"height", 140},
                {"icon_resource", "internal_icon_16x16"},
                {"step", 2.5},
                {"background_color", { {"r",16}, {"g",16}, {"b",16}, {"a",0} }},
                {"border_color",     { {"r",0},  {"g",0},  {"b",0},  {"a",0} }}
            }
        );
    rightMainFrame->addChild(vSlider);

    DisplayHandle vProgress =
        factory.createDisplayObjectFromJson(
            "ProgressBar",
            nlohmann::json{
                {"name", "mainFrame_vprogress_1"},
                {"type", "ProgressBar"},
                {"orientation", "vertical"},
                {"x", 554},
                {"y", 154},
                {"width", 16},
                {"height", 140},
                {"icon_resource", "internal_icon_16x16"},
                {"value", 25.0},
                {"step", 2.5},
                {"color", { {"r",0}, {"g",255}, {"b",0}, {"a",255} }},
                {"foreground_color", { {"r",128}, {"g",128}, {"b",128}, {"a",255} }},
                {"background_color", { {"r",16}, {"g",16}, {"b",16}, {"a",0} }},
                {"border_color",     { {"r",0},  {"g",0},  {"b",0},  {"a",0} }}
            }
        );
    rightMainFrame->addChild(vProgress);

    DisplayHandle vScroll =
        factory.createDisplayObjectFromJson(
            "ScrollBar",
            nlohmann::json{
                {"name", "mainFrame_vscrollbar_1"},
                {"type", "ScrollBar"},
                {"orientation", "vertical"},
                {"x", 534},
                {"y", 154},
                {"width", 16},
                {"height", 140},
                {"icon_resource", "internal_icon_16x16"},
                {"value", 50.0},
                {"step", 5.0}
            }
        );
    rightMainFrame->addChild(vScroll);


    // === Main Stage Controls ===================================================
    DisplayHandle mainStageButton =
        factory.createDisplayObjectFromJson(
            "Button",
            nlohmann::json{
                {"name", "main_stage_button"},
                {"type", "Button"},
                {"x", 5},
                {"y", 370},
                {"width", 150},
                {"height", 25},
                {"icon_resource", "internal_icon_16x16"},
                {"text", "[color=white]Go to [color=yellow]S[color=white]tage Two"},
                {"font_resource", "VarelaRound16"},
                {"font_size", 16},
                {"label_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"color", { {"r",96}, {"g",32}, {"b",16}, {"a",255} }}
            }
        );
    rootStage->addChild(mainStageButton);

    DisplayHandle hamburgerIcon =
        factory.createDisplayObjectFromJson(
            "IconButton",
            nlohmann::json{
                {"name", "main_stage_hamburger_iconbutton"},
                {"type", "IconButton"},
                {"x", 150},
                {"y", 8},
                {"width", 16},
                {"height", 16},
                {"color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"icon_resource", "internal_icon_16x16"},
                {"border", true},
                {"icon_index", 54}
            }
        );
    rootStage->addChild(hamburgerIcon);

    DisplayHandle arrowButton =
        factory.createDisplayObjectFromJson(
            "ArrowButton",
            nlohmann::json{
                {"name", "main_stage_arrowbutton"},
                {"type", "ArrowButton"},
                {"x", 182},
                {"y", 8},
                {"width", 16},
                {"height", 16},
                {"direction", "up"},
                {"color", { {"r",255}, {"g",255}, {"b",0}, {"a",255} }},
                {"icon_resource", "internal_icon_16x16"},
                {"border", false},
                {"border_color", { {"r",255}, {"g",255}, {"b",255}, {"a",128} }},
                {"background", false},
                {"background_color", { {"r",0}, {"g",0}, {"b",0}, {"a",64} }}
            }
        );
    rootStage->addChild(arrowButton);


    // === Decorative Boxes ======================================================
    DisplayHandle redishBox =
        factory.createDisplayObjectFromJson(
            "Box",
            nlohmann::json{
                {"name", "redishBox"},
                {"type", "Box"},
                {"x", 100.0},
                {"y", 100},
                {"width", 120},
                {"height", 80},
                {"color", { {"r",128}, {"g",16}, {"b",16}, {"a",255} }}
            }
        );
    rootStage->addChild(redishBox);

    DisplayHandle redishLabel =
        factory.createDisplayObjectFromJson(
            "Label",
            nlohmann::json{
                {"name", "redishBoxLabel"},
                {"type", "Label"},
                {"anchor_left", "left"},
                {"x", 105},
                {"anchor_top", "top"},
                {"y", 105},
                {"anchor_right", "right"},
                {"width", 110},
                {"anchor_bottom", "bottom"},
                {"height", 70},
                {"outline", true},
                {"wordwrap", true},
                {"auto_resize", true},
                {"text", "[color=white]Hello [bold][dropshadow]VarelaRound[/dropshadow][/bold], this is a 32 point font size."},
                {"resource_name", "VarelaRound32"},
                {"font_size", 32},
                {"alignment", "center"},
                {"foreground_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"border", true},
                {"border_color", { {"r",255}, {"g",255}, {"b",255}, {"a",64} }}
            }
        );
    redishBox->addChild(redishLabel);


    DisplayHandle greenishBox =
        factory.createDisplayObjectFromJson(
            "Box",
            nlohmann::json{
                {"name", "greenishBox"},
                {"type", "Box"},
                {"x", 150},
                {"y", 150},
                {"width", 180},
                {"height", 120},
                {"color", { {"r",16}, {"g",64}, {"b",16}, {"a",255} }}
            }
        );
    rootStage->addChild(greenishBox);

    DisplayHandle greenishLabel =
        factory.createDisplayObjectFromJson(
            "Label",
            nlohmann::json{
                {"name", "greenishBoxLabel"},
                {"type", "Label"},
                {"anchor_left", "left"},
                {"x", 155},
                {"anchor_top", "top"},
                {"y", 155},
                {"anchor_right", "right"},
                {"width", 170},
                {"anchor_bottom", "bottom"},
                {"height", 110},
                {"outline", true},
                {"wordwrap", true},
                {"auto_resize", false},
                {"max_width", 400},
                {"text", "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat"},
                {"resource_name", "internal_font_8x8"},
                {"alignment", "center"},
                {"foreground_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"border", true},
                {"border_color", { {"r",255}, {"g",255}, {"b",255}, {"a",64} }}
            }
        );
    greenishBox->addChild(greenishLabel);


    DisplayHandle blueishBox =
        factory.createDisplayObjectFromJson(
            "Box",
            nlohmann::json{
                {"name", "blueishBox"},
                {"type", "Box"},
                {"x", 180},
                {"y", 70},
                {"width", 250},
                {"height", 225},
                {"color", { {"r",50}, {"g",50}, {"b",200}, {"a",255} }}
            }
        );
    rootStage->addChild(blueishBox);

    DisplayHandle blueishLabel =
        factory.createDisplayObjectFromJson(
            "Label",
            nlohmann::json{
                {"name", "blueishBoxLabel"},
                {"type", "Label"},
                {"anchor_left", "left"},
                {"x", 185},
                {"anchor_top", "top"},
                {"y", 75},
                {"anchor_right", "right"},
                {"width", 240},
                {"anchor_bottom", "bottom"},
                {"height", 215},
                {"wordwrap", true},
                {"auto_resize", false},
                {"outline", true},
                {"text", "The [bold][color=lt_red]quick brown[color=white][/bold] fox jumps over the lazy dog by the river, sunlight catching on ripples as children laugh in the distance. A small breeze stirs the pages of an open book left on the bench, and a bell rings from the town square."},
                {"resource_name", "internal_ttf"},
                {"border", true},
                {"border_color", { {"r",255}, {"g",255}, {"b",255}, {"a",64} }},
                {"font_size", 16},
                {"alignment", "center"},
                {"foreground_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"max_width", 500}
            }
        );
    blueishBox->addChild(blueishLabel);


    DisplayHandle orangishBox =
        factory.createDisplayObjectFromJson(
            "Box",
            nlohmann::json{
                {"name", "orangishBox"},
                {"type", "Box"},
                {"x", 40},
                {"y", 200},
                {"width", 100},
                {"height", 75},
                {"color", { {"r",128}, {"g",64}, {"b",25}, {"a",255} }}
            }
        );
    rootStage->addChild(orangishBox);

    DisplayHandle orangishLabel =
        factory.createDisplayObjectFromJson(
            "Label",
            nlohmann::json{
                {"name", "orangeishBoxLabel"},
                {"type", "Label"},
                {"anchor_left", "left"},
                {"x", 45},
                {"anchor_top", "top"},
                {"y", 205},
                {"anchor_right", "right"},
                {"width", 90},
                {"anchor_bottom", "bottom"},
                {"height", 65},
                {"outline", true},
                {"wordwrap", true},
                {"auto_resize", false},
                {"max_width", 400},
                {"text", "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat"},
                {"resource_name", "internal_font_8x12"},
                {"alignment", "center"},
                {"foreground_color", { {"r",255}, {"g",255}, {"b",255}, {"a",255} }},
                {"border", true},
                {"border_color", { {"r",255}, {"g",255}, {"b",255}, {"a",64} }}
            }
        );
    orangishBox->addChild(orangishLabel);






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