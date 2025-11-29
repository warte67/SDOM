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
#include <SDOM/CAPI/SDOM_CAPI_Core.h>
#include <SDOM/CAPI/SDOM_CAPI_Version.h>

/* ---------------------------------------------------------------------------
   Variant 3 entry point (C API mode)
--------------------------------------------------------------------------- */
int SDOM_main_variant_3(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    // Reminder: once we formalize startup docs, describe how Variant 3 maps to SDOM_Init/Configure.

    // SDOM_Init() initializes the engine using default configuration values, so the engine is immediately ready.
    // SDOM_Configure() allows you to override any of those defaults at any time.

    // This is different from SDL, where Init does nothing to configure windows or rendering.
    // SDOM is structured as a higher-level engine with safe defaults and immediate usability.    

    if (!SDOM_Init(0))
    {
        const char* apiError = SDOM_GetError();
        std::cerr << "[Variant 3] ERROR: SDOM_Init(0) failed. "
                  << (apiError ? apiError : "(no error message)")
                  << std::endl;
        return 1;
    }

    SDOM_CoreConfig cfg{};
    if (SDOM_GetCoreConfig(&cfg))
    {
        cfg.windowWidth  = 1280.0f;
        cfg.windowHeight = 960.0f;
        cfg.pixelWidth   = 1.0f;
        cfg.pixelHeight  = 1.0f;
        (void)SDOM_Configure(&cfg);
    }
    else
    {
        std::cerr << "[Variant 3] WARNING: SDOM_GetCoreConfig failed; continuing with defaults." << std::endl;
    }

    const char* fullVersion = SDOM_GetVersionFullString();
    if (!fullVersion) {
        fullVersion = "(unknown version)";
    }

    std::cout << "[Variant 3] SDOM " << fullVersion << std::endl;

    SDOM_Quit();

    return 0;
}
