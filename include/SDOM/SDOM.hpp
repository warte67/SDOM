/*** SDOM.hpp **************************************
 *    ___ ___   ___  __  __   _              
 *   / __|   \ / _ \|  \/  | | |_  _ __ _ __ 
 *   \__ \ |) | (_) | |\/| |_| ' \| '_ \ '_ \
 *   |___/___/ \___/|_|  |_(_)_||_| .__/ .__/
 *                                |_|  |_|   
 *
 * The SDOM.hpp header file serves as a centralized location for 
 * defining commonly used types, global macros, and debugging 
 * utilities within the SDOM namespace. It includes type aliases, 
 * such as ID, and provides macros for error handling (ERROR), 
 * warnings (WARNING), and debugging (DEBUG) with formatted output. 
 * This file helps standardize and simplify code across the project. 
 * 
 * Note:
 *   SDOM.hpp includes both the umbrella header (SDOM_Front.hpp) for 
 *   implementation and an interface-safe header (SDOM_Interface.hpp) 
 *   for type definitions, macros, and utilities. This ensures you 
 *   have access to all SDOM features with a single include.  For 
 *   advanced usage, you may include individual SDOM headers as needed.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Released under the ZLIB License.
 * Original Author: Jay Faries (warte67)
 * 
 **********************/

#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
// SDL_mixer is optional; include only if available
#if defined(__has_include)
#  if __has_include(<SDL3_mixer/SDL_mixer.h>)
#    include <SDL3_mixer/SDL_mixer.h>
#    define SDOM_HAS_SDL_MIXER 1
#  else
#    define SDOM_HAS_SDL_MIXER 0
#  endif
#else
#  define SDOM_HAS_SDL_MIXER 0
#endif
#include <SDL3_ttf/SDL_ttf.h>

#include <any>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// #define SOL_ALL_AUTOMAGIC 0
// #include <sol/sol.hpp>
// #include <external/nlohmann/json.hpp>
#include <json.hpp>
#include <SDOM/SDOM_Geometry.hpp>

// Garbage Collection / Orphan Retention
constexpr int ORPHAN_GRACE_PERIOD = 5000; // default grace period for orphaned objects (in milliseconds)

/**
 * @namespace SDOM
 * @brief Contains all core classes and utilities for the SDOM library.
 */
namespace SDOM 
{

    // ======================================================================
    // 🧩 Global SDOM Configuration Flags
    // ======================================================================

    // ----------------------------------------------------------------------
    // Unit Test Enable Control
    // ----------------------------------------------------------------------
    #ifndef SDOM_ENABLE_ALL_UNIT_TESTS
        /**
         * @brief Globally enables or disables all registered UnitTests in SDOM.
         *
         * @details
         * When `true`, every test module and sub-test declared via the SDOM
         * UnitTest framework will be discovered and executed during startup.
         * When `false`, test registration is still compiled, but none of them
         * will automatically run unless explicitly invoked by the developer
         * (e.g., through a Lua call or manual trigger in C++).
         *
         * ---
         * ⚙️ **Typical Usage**
         *
         * **Enable All Tests (default):**
         * ```bash
         * cmake -DSDOM_ENABLE_ALL_UNIT_TESTS=ON ..
         * ```
         *
         * **Disable All Tests (for release builds):**
         * ```bash
         * cmake -DSDOM_ENABLE_ALL_UNIT_TESTS=OFF ..
         * ```
         *
         * ---
         * 🧪 **Notes**
         * - Intended for toggling between *development/debug* and *release*
         *   configurations.
         * - Disabling tests does *not* remove them from compilation; it only
         *   prevents automatic execution.
         * - Useful for CI pipelines or embedded environments where runtime
         *   test harnesses are not needed.
         */
        inline constexpr bool ENABLE_ALL_UNIT_TESTS = true;
    #else
        inline constexpr bool ENABLE_ALL_UNIT_TESTS = SDOM_ENABLE_ALL_UNIT_TESTS;
    #endif    


    /**
     * @section SDOM_TestOutputConfig
     * @brief Global compile-time configuration flags controlling test verbosity.
     *
     * These constants define the verbosity level for both internal diagnostic output
     * and the UnitTest reporting system. They can be overridden at compile time
     * using either **CMake options** or direct **compiler definitions**.
     *
     * **Example (CMake):**
     * ```
     * cmake -DSDOM_VERBOSE_TEST_OUTPUT=ON -DSDOM_QUIET_TEST_MODE=OFF ..
     * ```
     *
     * **Example (g++):**
     * ```
     * g++ -DSDOM_VERBOSE_TEST_OUTPUT=true -DSDOM_QUIET_TEST_MODE=false ...
     * ```
     *
     * ---
     * ⚙️ **Behavior Matrix**
     *
     * | Mode        | VERBOSE_TEST_OUTPUT | QUIET_TEST_MODE | Description |
     * |--------------|--------------------|-----------------|--------------|
     * | **Verbose** *(default)* | `true`  | `false` | Displays all modules and individual test results, including passes, with full color and detail. |
     * | **Compact**              | `false` | `false` | Displays only per-module summaries and failing results — minimal console noise. |
     * | **Quiet**                | any     | `true`  | Suppresses all output unless a test fails; in that case, the entire failing module’s test tree and its errors are printed. |
     *
     * ---
     * 🔧 **Notes**
     * - `QUIET_TEST_MODE` always takes precedence — if enabled, it overrides
     *   `VERBOSE_TEST_OUTPUT`.
     * - Designed for build servers, CI systems, or silent regression runs where
     *   concise output is preferred.
     * - These flags do **not** affect SDL, Lua, or other runtime logging systems.
     */

    // ----------------------------------------------------------------------
    // Verbose Output Control
    // ----------------------------------------------------------------------
    #ifndef SDOM_VERBOSE_TEST_OUTPUT
        /**
         * @brief Enables detailed console output for all tests.
         * @details
         * When `true`, every test — including passing ones — is displayed
         * with its object name, test title, and result indicator.
         */
        inline constexpr bool VERBOSE_TEST_OUTPUT = true;
    #else
        inline constexpr bool VERBOSE_TEST_OUTPUT = SDOM_VERBOSE_TEST_OUTPUT;
    #endif

    // ----------------------------------------------------------------------
    // Quiet Mode Control (overrides Verbose)
    // ----------------------------------------------------------------------
    #ifndef SDOM_QUIET_TEST_MODE
        /**
         * @brief Suppresses console output for passing tests and modules.
         * @details
         * When `true`, no test output is shown unless one or more sub-tests fail.
         * In that case, the entire failing module’s test tree and associated
         * error messages are printed.
         *
         * This flag takes precedence over `VERBOSE_TEST_OUTPUT`.
         */
        inline constexpr bool QUIET_TEST_MODE = false;
    #else
        inline constexpr bool QUIET_TEST_MODE = SDOM_QUIET_TEST_MODE;
    #endif


    // ======================================================================
    // 🧩 Core Runtime Timing Defaults
    // ======================================================================
    // --- Compile-time metering and throttling defaults --- //
    // Global default meter interval (milliseconds) for mergeable events.
    // Used unless an EventType overrides via its per-type policy.
    inline constexpr Uint32 SDOM_EVENT_METER_MS_DEFAULT = 10; // ~100 Hz
    // Hover hit-test throttle (milliseconds). Pointer hit-tests during motion
    // are limited to at most this rate to reduce subtree walks.
    inline constexpr Uint32 SDOM_HOVER_HITTEST_MS = 5; // ~200 Hz
    /**
     * @brief Custom exception class for SDOM errors.
     * @details Stores an error message, file name, and line number for debugging.

    * Use with the ERROR macro for standardized error reporting.
    */
    // Exception class definition
    class Exception : public std::exception 
    {
        public:
            Exception(const std::string& message, const std::string& file, int line)
                : message_(message), file_(file), line_(line) {}
            const char* what() const noexcept override { return message_.c_str(); }
            std::string getFile() const { return file_; }
            int getLine() const { return line_; }
        private:
            std::string message_;
            std::string file_;
            int line_;
    };    

    // // stand alone SDOM functions
    // static const char* version();
    // static const char* compilationDate();

    // class Core;
    // class Factory;
    // class Stage;

    // void quit();
    // void shutdown();
    // Core& getCore();
    // Factory& getFactory();
    // Stage* getStage();
    // DomHandle getStageHandle();
    // SDL_Renderer* getRenderer();
    // SDL_Window* getWindow();
    // SDL_Texture* getTexture();


    // Geometry primitives (AnchorPoint, Bounds, helpers) now live in
    // SDOM_Geometry.hpp so the definitions can be shared with the C API
    // bindings and tooling without duplicating documentation here.

} // namespace SDOM

/**
 * @brief Throws an SDOM exception with file and line info.
 * @param message Error message to report.
 */
// Macro for throwing exceptions
#include <sstream>

#define ERROR(msg)                                                          \
    do {                                                                    \
        std::ostringstream sdom_err_stream__;                               \
        sdom_err_stream__ << msg;                                           \
        throw SDOM::Exception(sdom_err_stream__.str(), __FILE__, __LINE__); \
    } while (0)

    
/**
 * @brief Shows a warning message with file and line info.
 * @param message Warning message to display.
 */
// Macro for showing warnings
#define WARNING(message) SDOM::showWarning(message, __FILE__, __LINE__)

/**
 * @brief Logs a debug message in green text.
 * @param message Debug message to display.
 */
// Macro for debug logging (statement-safe, parenthesized parameter)
#define DEBUG_LOG(...) do { \
    std::ostringstream _sdom_dbg_oss; \
    _sdom_dbg_oss << __VA_ARGS__; \
    std::cout << CLR::GREEN << "[DEBUG] " << _sdom_dbg_oss.str() << CLR::RESET << std::endl; \
} while(0)

/**
 * @brief Logs a failure message in red text.
 * @param message Failure message to display.
 */
// Macro for failure logging (statement-safe, parenthesized parameter)
#define FAIL(...) do { \
    std::ostringstream _sdom_dbg_oss; \
    _sdom_dbg_oss << __VA_ARGS__; \
    std::cout << CLR::RED << "[FAILED] " << _sdom_dbg_oss.str() << CLR::RESET << std::endl; \
} while(0)

/**
 * @brief Logs an informational message to stdout.
 * @param message Informational message to display.
 */
// Macro for informational logging (statement-safe, parenthesized parameter)
#define INFO(...) do { \
    std::ostringstream _sdom_dbg_oss; \
    _sdom_dbg_oss << __VA_ARGS__; \
    std::cout << CLR::YELLOW << "[INFO] " << (_sdom_dbg_oss.str()) << CLR::RESET << std::endl; } while(0)

// Lua-specific informational logging (only active when DEBUG_LUA_TESTS == 1)
#define LUA_INFO(...) do { if (DEBUG_LUA_TESTS) { std::ostringstream _sdom_lua_dbg_oss; _sdom_lua_dbg_oss << __VA_ARGS__; std::cout << CLR::YELLOW << "[LUA-INFO] " << (_sdom_lua_dbg_oss.str()) << CLR::RESET << std::endl; } } while(0)

/**
 * @brief Logs a debug error (non-fatal) with file and line info in dark text.
 * @param message Debug message to include.
 */
// Macro for debug errors (statement-safe)
#define DEBUG_ERR(message) do { std::cout << CLR::DARK << \
    "\n      Debug: " << \
    CLR::YELLOW << (message) << std::endl << CLR::DARK << \
    "      File:  " << CLR::ORANGE << __FILE__  << CLR::DARK << std::endl << \
    "      Line:  " << CLR::ORANGE  << __LINE__ << CLR::RESET << "\n" << std::endl; } while(0)

/**
 * @brief Asserts that two values are equal; throws an error if not.
 * @param val Value to test.
 * @param expected Expected value.
 */
// Assertion macro for testing equality
#define ASSERT_EQ(val, expected) \
    if ((val) != (expected)) { \
        ERROR("Assertion failed: " + std::string(#val) + " == " + std::string(#expected) \
                  + ", actual: " + std::to_string((val)) + ", expected: " + std::to_string((expected))); \
    }

#include <SDOM/SDOM_SDL_Utils.hpp>
#include <SDOM/SDOM_CLR.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_AssetHandle.hpp>
#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_Utils.hpp>
// #include <SDOM/SDOM_Version.hpp>

#ifndef SDOM_USE_INDIVIDUAL_HEADERS
    // SDOM_Front.hpp is an umbrella header meant for implementation files only.
    // Including it from SDOM.hpp can create circular includes (SDOM.hpp -> SDOM_Front.hpp -> SDOM_Core.hpp -> SDOM.hpp)
    // which may leave class definitions incomplete during header processing. Consumers who want the full
    // API should include SDOM_Front.hpp directly in .cpp files. Keep this block intentionally empty here
    // to avoid accidental circular include dependencies.
#endif


