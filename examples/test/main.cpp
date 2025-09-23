/**
 * @file main.cpp
 * @author Jay Faries (https://github.com/warte67/SDOM)
 * @brief Main entry point for the SDOM example test application.

 * @date 2025-09-19
 * 
 * @copyright Copyright (c) 2025
 * 
//  */

#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_Event.hpp>
#include <SDOM/SDOM_IUnitTest.hpp>
#include <SDOM/SDOM_IDataObject.hpp>

#include "Box.hpp"
#include "UnitTests.hpp"

using namespace SDOM;


int main() 
{
    std::cout << CLR::YELLOW << "Hello, SDOM!" << CLR::RESET << std::endl;

    // Fetch the Core singleton
    Core& core = getCore();

    // register any custom DisplayObject types before configuring the Core
    core.getFactory().registerType("Box", TypeCreators{
        Box::CreateFromJson,
        Box::CreateFromInitStruct
    });

    // Configure the Core from JSON
    core.configureFromJson(R"({
        "Core": {
            "windowWidth": 1200,
            "windowHeight": 800,
            "pixelWidth": 2,
            "pixelHeight": 2,
            "allowTextureResize": true,
            "preserveAspectRatio": true,
            "rendererLogicalPresentation": "SDL_LOGICAL_PRESENTATION_LETTERBOX",
            "windowFlags": "SDL_WINDOW_RESIZABLE",
            "pixelFormat": "SDL_PIXELFORMAT_RGBA8888",
            "color": { "r": 0, "g": 0, "b": 0, "a": 255 },
            "rootStage": "mainStage",
            "children": [
                {
                    "rootStage": "mainStage",
                    "type": "Stage",
                    "name": "mainStage",
                    "color": { "r": 32, "g": 8, "b": 4, "a": 255 },
                        "children": [
                            {
                                "type": "Box",
                                "name": "box1",
                                "x": 100,
                                "y": 100,
                                "width": 120,
                                "height": 80,
                                "color": { "r": 200, "g": 50, "b": 50, "a": 255 }
                            },
                            {
                                "type": "Box",
                                "name": "box2",
                                "x": 300,
                                "y": 150,
                                "width": 80,
                                "height": 120,
                                "color": { "r": 50, "g": 200, "b": 50, "a": 255 }
                            }
                        ]
                },
                {
                    "type": "Stage",
                    "name": "stageTwo",
                    "color": { "r": 16, "g": 32, "b": 8, "a": 255 }
                },
                {
                    "type": "Stage",
                    "name": "stageThree",
                    "color": { "r": 8, "g": 16, "b": 32, "a": 255 }
                }
            ]
        }
    })");

    core.registerOnInit([]() {
        // Post SDL user initialization here
        return true; 
    });

    core.registerOnQuit([]() {
        // std::cout << CLR::GREEN << "Custom OnQuit called!" << CLR::RESET << std::endl;
    });

    core.registerOnEvent([](const Event& event) {
        // Handle events here
        // std::cout << CLR::GREEN << "Event received!" << CLR::RESET << std::endl;
    });

    core.registerOnUpdate([](float fElapsedTime) {
        // Update logic here
        // std::cout << CLR::GREEN << "Custom Update called! Elapsed time: " << fElapsedTime << " seconds" << CLR::RESET << std::endl;
    });

    core.registerOnRender([]() {
        // Custom rendering logic here
        // std::cout << CLR::GREEN << "Custom Render called!" << CLR::RESET << std::endl;
    });

    core.registerOnUnitTest([]() 
    {
        bool allTestsPassed = true;
        bool result = true;

        // Run Custom Unit Tests
        allTestsPassed &= Core_UnitTests();
        allTestsPassed &= Factory_UnitTests();
        allTestsPassed &= IDisplayObject_UnitTests();
        allTestsPassed &= Stage_UnitTests();

        // User Test One
        result = UnitTests::run("User", "The first user test", []() { return true; });
        if (!result) {
            std::cout << CLR::indent() << "Something Failed (appropriate debug text here)" << CLR::RESET << std::endl;
        } 
        allTestsPassed &= result;

        // User Test Two
        result = UnitTests::run("User", "The second user test", []()  { return true; });
        if (!result) {
            std::cout << CLR::indent() << "Something Failed (appropriate debug text here)" << CLR::RESET << std::endl;
        } 
        allTestsPassed &= result;


        return allTestsPassed;
    });

    core.registerOnWindowResize([](int newWidth, int newHeight) 
    {
        // std::cout << "Window Resized--Width: " << newWidth << "  Height: " << newHeight << std::endl;
    });

    core.run();
    return 0;
} // END main()