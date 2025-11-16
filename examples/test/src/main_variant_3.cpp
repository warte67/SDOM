/**
 * @file main_variant_3.cpp
 * @brief SDOM Startup Variant 3 — Minimal C API example.
 *
 * This variant demonstrates:
 *   - Pure C API initialization
 *   - Basic CoreConfig retrieval/modification
 *   - Minimal startup/shutdown
 */

#include <iostream>
#include <SDOM/SDOM_CAPI.h>

/* ---------------------------------------------------------------------------
   Variant 3 entry point (C API mode)
--------------------------------------------------------------------------- */
int SDOM_main_variant_3(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    bool ok = true;

    // -----------------------------------------
    // Initialize SDOM runtime
    // -----------------------------------------
    if (!SDOM_Init())
    {
        std::cerr << "[SDOM] ERROR: SDOM_Init() failed.\n";
        return 1;
    }

    // -----------------------------------------
    // Get current config, modify some defaults
    // -----------------------------------------
    SDOM_CoreConfig cfg;
    SDOM_GetCoreConfig(&cfg);

    cfg.windowWidth  = 1280.0f;
    cfg.windowHeight = 960.0f;
    cfg.pixelWidth   = 1.0f;
    cfg.pixelHeight  = 1.0f;

    // Apply configuration
    SDOM_Configure(&cfg);

    // -----------------------------------------
    // Run main loop (no scene loaded)
    // -----------------------------------------
    // bool ok = SDOM_Run();  // not yet ready

    // -----------------------------------------
    // Clean up the runtime
    // -----------------------------------------
    SDOM_Quit();

    return ok ? 0 : 1;
}
