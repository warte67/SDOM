// #include <SDOM/SDOM_CAPI.hpp>
// #include <SDOM/SDOM_Core.hpp>
// #include <SDOM/SDOM_Stage.hpp>
// #include <iostream>
// #include <atomic>
// #include <thread>

// // Forward declarations for C++ unit tests
// namespace SDOM {
//     bool Core_UnitTests();
//     bool Variant_UnitTests();
//     bool DataRegistry_UnitTests();
// }

// // Simple C-style callback state used by the C API test.
// static std::atomic<int> g_frames{0};

// extern "C" void capi_on_update(float /*dt*/)
// {
//     g_frames.fetch_add(1, std::memory_order_relaxed);
//     // After a few frames, request shutdown via the C API
//     if (g_frames.load(std::memory_order_relaxed) >= 3) {
//         SDOM_Quit();
//     }
// }

// extern "C" void capi_on_render(void)
// {
//     // no-op; just ensure render callback can be registered and invoked
// }

// extern "C" void capi_on_quit(void)
// {
//     // no-op
// }

// namespace SDOM {

// bool Run_Core_CAPI_Tests()
// {
//     std::cout << "[CAPI Tests] Starting SDOM C API basic integration test...\n";

//     // Ensure any previous state is cleared
//     g_frames.store(0);

//     // Register callbacks using the C API
//     SDOM_RegisterOnUpdate(&capi_on_update);
//     SDOM_RegisterOnRender(&capi_on_render);
//     SDOM_RegisterOnQuit(&capi_on_quit);

//     // Initialize runtime and run a short main loop that quits after a few frames
//     SDOM_Init();

//     SDOM_CoreConfig_c cfg;
//     // Keep defaults sensible; zero means Core will pick good defaults.
//     cfg.windowWidth = 0.0f;
//     cfg.windowHeight = 0.0f;
//     cfg.pixelWidth = 0.0f;
//     cfg.pixelHeight = 0.0f;
//     cfg.preserveAspectRatio = 1;
//     cfg.allowTextureResize = 1;
//     cfg.rendererLogicalPresentation = 0;
//     cfg.windowFlags = 0;
//     cfg.pixelFormat = 0;
//     cfg.color = {255,255,255,255};

//     // This call will block until SDOM_Quit() is called from our update callback
//     SDOM_Run(&cfg);

//     // Basic assertions: ensure we saw some frames
//     int seen = g_frames.load(std::memory_order_relaxed);
//     bool capi_ok = (seen >= 1);
//     std::cout << "[CAPI Tests] Frames observed during run: " << seen << " -> " << (capi_ok ? "OK" : "FAIL") << "\n";

//     // Unregister callbacks
//     SDOM_RegisterOnUpdate(nullptr);
//     SDOM_RegisterOnRender(nullptr);
//     SDOM_RegisterOnQuit(nullptr);

//     // Now run the three C++ unit tests the repo uses as core checks
//     bool core_ok = Core_UnitTests();
//     std::cout << "[C++ Core_UnitTests] -> " << (core_ok ? "PASS" : "FAIL") << "\n";

//     bool variant_ok = Variant_UnitTests();
//     std::cout << "[C++ Variant_UnitTests] -> " << (variant_ok ? "PASS" : "FAIL") << "\n";

//     bool datareg_ok = DataRegistry_UnitTests();
//     std::cout << "[C++ DataRegistry_UnitTests] -> " << (datareg_ok ? "PASS" : "FAIL") << "\n";

//     bool all_ok = capi_ok && core_ok && variant_ok && datareg_ok;
//     std::cout << "[CAPI Tests] Overall result: " << (all_ok ? "PASS" : "FAIL") << "\n";
//     return all_ok;
// }

// } // namespace SDOM
