/**
 * @file main_variant_2.cpp
 * @brief MAIN_VARIANT 2 — Pure C++ front-end initialization + UnitTests
 *
 * This file is extracted directly from the MAIN_VARIANT == 2 section of main.cpp.
 * No behavior has been modified — only relocated into its own compilation unit.
 */

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

#include "Box.hpp"
#include "UnitTests.hpp"

using namespace SDOM;


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
}



// ============================================================================
// MAIN_VARIANT 2 — Pure C++ initialization path
// ============================================================================

int SDOM_main_variant_2(int argc, char** argv)
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

    // --- Built-in asset definitions -------------------------------------------

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
    AssetHandle externalIcon8x8_handle =
        factory.createAssetObject("SpriteSheet", external_icon_8x8);


    // Register minimal types needed by tests
    core.getFactory().registerDisplayObjectType("Box", TypeCreators{
        Box::CreateFromInitStruct,
        Box::CreateFromJson
    });


    // ============================================================================
    // Create the main Stage
    // ============================================================================
    SDOM::Stage::InitStruct stage_init;
    stage_init.name = "mainStage";
    stage_init.type = "Stage";
    stage_init.color = SDL_Color{32, 8, 4, 255};
    DisplayHandle rootStage = core.createDisplayObject("Stage", stage_init);
    core.setRootNode(rootStage);



    // ============================================================================
    // Create RIGHT FRAME
    // ============================================================================
    DisplayHandle rightMainFrame =
        factory.createDisplayObjectFromJson(
            "Frame",
            nlohmann::json{
                {"name", "rightMainFrame"},
                {"type", "Frame"},
                {"text", "Hello World."},
                {"x", 300},
                {"y", 5},
                {"width", 295},
                {"height", 390},
                {"icon_resource", "internal_icon_16x16"},
                {"color", { {"r",64}, {"g",32}, {"b",32}, {"a",255} }}
            }
        );
    rootStage->addChild(rightMainFrame);



    // ============================================================================
    // MAIN FRAME GROUP
    // ============================================================================
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



    // ============================================================================
    // RADIO BUTTONS (inside mainGroup)
    // ============================================================================
    {
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
                    {"checked", true}
                }
            );
        mainGroup->addChild(rb2);

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
    }



    // ============================================================================
    // CHECK BUTTONS (in rightMainFrame)
    // ============================================================================
    {
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
    }



    // ============================================================================
    // Additional RadioButtons on Frame
    // ============================================================================
    {
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
    }



    // ============================================================================
    // Tristate Button
    // ============================================================================
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



    // ============================================================================
    // Horizontal Slider / Progress / Scroll
    // ============================================================================
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



    // ============================================================================
    // Vertical Slider / Progress / Scroll
    // ============================================================================
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



    // ============================================================================
    // Main Stage Controls (Buttons, IconButton, ArrowButton)
    // ============================================================================
    {
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
    }



    // ============================================================================
    // Decorative Boxes + Labels
    // ============================================================================
    {
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
    }

    // // ============================================================================
    // // Print tree and run
    // // ============================================================================
    // rootStage->printTree();

    bool ok = core.run();
    return ok ? 0 : 1;
}

