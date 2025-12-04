/**
 * @file main_variant_6.cpp
 * @brief SDOM Variant 6 — SDL-style manual-loop CAPI example.
 */

#include <iostream>
#include <string>
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/CAPI/SDOM_CAPI_Event.h>
#include <SDOM/SDOM_Core.hpp>
#include <SDOM/SDOM_Factory.hpp>

#include "Box.hpp"

using namespace SDOM;

extern bool Main_UnitTests();

int SDOM_main_variant_6(int argc, char** argv)
{
    std::cout << "[Variant 6] Running SDL-style main loop.\n";
    // Initialize SDOM
    if (!SDOM_Init(0))
    {
        std::cerr << "[Variant 6] SDOM_Init failed: "
                  << SDOM_GetError() << std::endl;
        return 1;
    }

    // Register custom type "Box"
    Core& core = Core::getInstance();
    core.getFactory().registerDisplayObjectType("Box", TypeCreators{
        Box::CreateFromInitStruct,
        Box::CreateFromJson
    });

    // Load configuration
    if (!SDOM_LoadDomFromJsonFile("json/config.json"))
    {
        std::cerr << "[Variant 6] Failed to load DOM JSON: "
                  << SDOM_GetError() << std::endl;
        SDOM_Quit();
        return 1;
    }

    // Register callbacks
    SDOM_RegisterOnUnitTest((void*)Main_UnitTests);

    bool running = true;
    int exitCode = 0;

    // === SDL-STYLE MAIN LOOP ===
    while (running)
    {
        SDOM_Event evt{};

        // SDOM_PollEvents(&evt);
        // --- Inner event pump ---
        while (SDOM_PollEvents(&evt))
        {
            SDOM_EventType type = SDOM_EventType_None;

            if (SDOM_GetEventType(&evt, &type))
            {
                if (type == SDOM_EventType_Quit)
                {
                    std::cerr << "[Variant 6] Quit event received." << std::endl;
                    running = false;
                }
                if (type == SDOM_EventType_KeyDown)
                {
                    int key_code = SDOM_GetEventKeycode(&evt);
                    std::cerr << "[Variant 6] KeyDown event received with key code: " << key_code << std::endl;
                    std::cerr << "[Variant 6] SDLK_Q: " << SDLK_Q << std::endl;

                    if (key_code == SDLK_Q)
                    {
                        std::cerr << "[Variant 6] (SDLK_Q) Quit event received." << std::endl;
                        running = false;
                    }
                }
                // std::cerr << "[Variant 6] PollEvents returned event: " << type << std::endl;
            }
        }

        if (!running) break;

        // --- Frame phases ---
        if (!SDOM_Update())
        {
            std::cerr << "[Variant 6] Update error: " << SDOM_GetError() << std::endl;
            exitCode = 1;
            break;
        }

        if (!SDOM_Render())
        {
            std::cerr << "[Variant 6] Render error: " << SDOM_GetError() << std::endl;
            exitCode = 1;
            break;
        }

        if (!SDOM_Present())
        {
            std::cerr << "[Variant 6] Present error: " << SDOM_GetError() << std::endl;
            exitCode = 1;
            break;
        }
    }

    // Clean shutdown
    SDOM_Quit();
    if (exitCode == 0)
        std::cout << "[Variant 6] SDL-style loop exited cleanly.\n";
    else
        std::cout << "[Variant 6] SDL-style loop terminated due to errors.\n";
    return exitCode;
}
