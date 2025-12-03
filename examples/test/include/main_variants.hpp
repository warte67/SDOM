/**
 * @file main_variants.hpp
 * @brief Declarations for SDOM startup variants used by the unified main.cpp
 *
 * Each variant is implemented in its own translation unit:
 *   - main_variant_1.cpp
 *   - main_variant_2.cpp
 *   - main_variant_3.cpp
 *   - main_variant_4.cpp
 *   - main_variant_5.cpp
 *   - main_variant_6.cpp
 *
 * These functions behave exactly like a standalone `main(argc, argv)`
 * but are invoked by the unified command-line dispatcher.
 */

#pragma once

// Variant 1 — Lua-driven startup mode (existing legacy path)
int SDOM_main_variant_1(int argc, char** argv);

// Variant 2 — C++ front-end, JSON constructors, fast test mode
int SDOM_main_variant_2(int argc, char** argv);

// Variant 3 — C API bootstrap mode (SDOM_Init / SDOM_Configure)
int SDOM_main_variant_3(int argc, char** argv);

// Variant 4 — JSON config loader (builds DOM from config.json)
int SDOM_main_variant_4(int argc, char** argv);

// Variant 5 — CAPI Main Loop (pollEvents, update, render, present)
int SDOM_main_variant_5(int argc, char** argv);

// Variant 6 — Minimal manual main loop with SDL_EVENT_QUIT detection
int SDOM_main_variant_6(int argc, char** argv);
