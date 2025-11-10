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
#include <SDL3_mixer/SDL_mixer.h>
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
#include <sol/sol.hpp>

constexpr bool DEBUG_REGISTER_LUA = false;  // set to true to enable verbose Lua registration logs
// #define UNIT_TESTS_ENABLED true


// Enable verbose Lua-related test/info logs when set to 1. Default is off.
#ifndef DEBUG_LUA_TESTS
#define DEBUG_LUA_TESTS 0
#endif

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
        inline constexpr bool ENABLE_ALL_UNIT_TESTS = false;
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


    /**
     * @enum AnchorPoint
     * @brief Reference points for anchoring child edges to a parent.
     * @details
     * Specifies where each edge of a child display object should be anchored
     * relative to its parent, enabling flexible positioning and layout.
     * 
     * The main anchor points correspond to corners, edges, and the center of the parent.
     * Aliases are provided for convenience and readability.
     */
    enum class AnchorPoint : uint8_t
    {
        // Main anchor points
        DEFAULT         = 0,    ///< Default anchor point (same as TOP_LEFT)
        TOP_LEFT        = 0,    ///< Top-left corner of the parent
        TOP_CENTER      = 1,    ///< Top-center edge of the parent
        TOP_RIGHT       = 2,    ///< Top-right corner of the parent
        MIDDLE_LEFT     = 3,    ///< Middle-left edge of the parent
        MIDDLE_CENTER   = 4,    ///< Center of the parent
        MIDDLE_RIGHT    = 5,    ///< Middle-right edge of the parent
        BOTTOM_LEFT     = 6,    ///< Bottom-left corner of the parent
        BOTTOM_CENTER   = 7,    ///< Bottom-center edge of the parent
        BOTTOM_RIGHT    = 8,    ///< Bottom-right corner of the parent

        /// @name Aliases for anchor points
        ///@{
        LEFT            = MIDDLE_LEFT,   ///< Alias for MIDDLE_LEFT
        CENTER          = MIDDLE_CENTER, ///< Alias for MIDDLE_CENTER
        RIGHT           = MIDDLE_RIGHT,  ///< Alias for MIDDLE_RIGHT
        TOP             = TOP_CENTER,    ///< Alias for TOP_CENTER
        MIDDLE          = MIDDLE_CENTER, ///< Alias for MIDDLE_CENTER
        BOTTOM          = BOTTOM_CENTER, ///< Alias for BOTTOM_CENTER
        LEFT_TOP        = TOP_LEFT,      ///< Alias for TOP_LEFT
        CENTER_TOP      = TOP_CENTER,    ///< Alias for TOP_CENTER
        RIGHT_TOP       = TOP_RIGHT,     ///< Alias for TOP_RIGHT
        LEFT_MIDDLE     = MIDDLE_LEFT,   ///< Alias for MIDDLE_LEFT
        CENTER_MIDDLE   = MIDDLE_CENTER, ///< Alias for MIDDLE_CENTER
        RIGHT_MIDDLE    = MIDDLE_RIGHT,  ///< Alias for MIDDLE_RIGHT
        LEFT_BOTTOM     = BOTTOM_LEFT,   ///< Alias for BOTTOM_LEFT
        CENTER_BOTTOM   = BOTTOM_CENTER, ///< Alias for BOTTOM_CENTER
        RIGHT_BOTTOM    = BOTTOM_RIGHT,  ///< Alias for BOTTOM_RIGHT
        ///@}
    };

    /*
     * Note: Anchor name normalization
     * --------------------------------
     * Canonicalization of user-provided anchor strings is performed by
     * the helper function SDOM::normalizeAnchorString(const std::string&),
     * implemented in `src/SDOM_Utils.cpp`.
     *
     * Behavior summary:
     *  - Strips whitespace and lower-cases the input.
     *  - Recognizes a single operator character joining two keywords
     *    (one of: '-', '|', '&', '+', ',') and returns the joined form
     *    using '_' as the separator (e.g. "top+left" -> "top_left").
     *  - Validates keyword pairs (e.g. disallows "top_bottom" or
     *    "left_right") and returns an empty string while emitting a
     *    WARNING on invalid inputs.
     *
     * The Factory and other parsers call this function before looking up
     * entries in `stringToAnchorPoint_` so consumers can pass flexible
     * anchor names (spaces, different joiners, mixed case) and still
     * resolve to the canonical enum value.
     */

    /**
     * @brief Maps AnchorPoint enum values to their corresponding string names.
     * @details
     * Used for serialization, debugging, and user interface display.
     * Only the main anchor points are included; aliases are omitted for clarity.
     *
     * Example usage:
     *   std::string name = anchorPointToString_.at(AnchorPoint::TOP_LEFT); // "top_left"
     */
    inline static const std::unordered_map<AnchorPoint, std::string> anchorPointToString_ = {
        { AnchorPoint::TOP_LEFT,     "top_left" },
        { AnchorPoint::TOP_CENTER,   "top_center" },
        { AnchorPoint::TOP_RIGHT,    "top_right" },
        { AnchorPoint::MIDDLE_LEFT,  "middle_left" },
        { AnchorPoint::MIDDLE_CENTER,"middle_center" },
        { AnchorPoint::MIDDLE_RIGHT, "middle_right" },
        { AnchorPoint::BOTTOM_LEFT,  "bottom_left" },
        { AnchorPoint::BOTTOM_CENTER,"bottom_center" },
        { AnchorPoint::BOTTOM_RIGHT, "bottom_right" }
    };

    /**
     * @brief Maps string names to AnchorPoint enum values.
     * @details
     * Supports multiple naming conventions for flexibility and user convenience:
     * - Standard names (e.g., "top_left", "center")
     * - Reversed keys (e.g., "center_middle", "right_top")
     * - Hyphenated names (e.g., "top-left", "bottom-center")
     * - Boolean OR style (e.g., "top|left", "center|bottom")
     *
     * Supported mappings:
     * | String Key        | AnchorPoint Value   |
     * |------------------|--------------------|
     * | "default"        | DEFAULT            |
     * | "left"           | LEFT               |
     * | "center"         | CENTER             |
     * | "right"          | RIGHT              |
     * | "top"            | TOP                |
     * | "top_left"       | TOP_LEFT           |
     * | "top_center"     | TOP_CENTER         |
     * | "top_right"      | TOP_RIGHT          |
     * | "middle"         | MIDDLE             |
     * | "middle_left"    | MIDDLE_LEFT        |
     * | "middle_center"  | MIDDLE_CENTER      |
     * | "middle_right"   | MIDDLE_RIGHT       |
     * | "bottom"         | BOTTOM             |
     * | "bottom_left"    | BOTTOM_LEFT        |
     * | "bottom_center"  | BOTTOM_CENTER      |
     * | "bottom_right"   | BOTTOM_RIGHT       |
     * | "center_middle"  | MIDDLE_CENTER      |
     * | "right_middle"   | MIDDLE_RIGHT       |
     * | "left_middle"    | MIDDLE_LEFT        |
     * | "center_top"     | TOP_CENTER         |
     * | "right_top"      | TOP_RIGHT          |
     * | "left_top"       | TOP_LEFT           |
     * | "center_bottom"  | BOTTOM_CENTER      |
     * | "right_bottom"   | BOTTOM_RIGHT       |
     * | "left_bottom"    | BOTTOM_LEFT        |
     *
     * Example usage:
     *   AnchorPoint ap = stringToAnchorPoint_.at("center_middle"); // AnchorPoint::MIDDLE_CENTER
     */
    inline static const std::unordered_map<std::string, AnchorPoint> stringToAnchorPoint_ = {
        /// @name Standard names
        ///@{
        { "default",        AnchorPoint::DEFAULT },
        { "left",           AnchorPoint::LEFT },
        { "center",         AnchorPoint::CENTER },
        { "right",          AnchorPoint::RIGHT },
        { "top",            AnchorPoint::TOP },
        { "top_left",       AnchorPoint::TOP_LEFT },
        { "top_center",     AnchorPoint::TOP_CENTER },
        { "top_right",      AnchorPoint::TOP_RIGHT },
        { "middle",         AnchorPoint::MIDDLE },
        { "middle_left",    AnchorPoint::MIDDLE_LEFT },
        { "middle_center",  AnchorPoint::MIDDLE_CENTER },
        { "middle_right",   AnchorPoint::MIDDLE_RIGHT },
        { "bottom",         AnchorPoint::BOTTOM },
        { "bottom_left",    AnchorPoint::BOTTOM_LEFT },
        { "bottom_center",  AnchorPoint::BOTTOM_CENTER },
        { "bottom_right",   AnchorPoint::BOTTOM_RIGHT },
        ///@}

        /// @name Reversed keys for user convenience
        ///@{
        { "center_middle",  AnchorPoint::MIDDLE_CENTER },
        { "right_middle",   AnchorPoint::MIDDLE_RIGHT },
        { "left_middle",    AnchorPoint::MIDDLE_LEFT },
        { "center_top",     AnchorPoint::TOP_CENTER },
        { "right_top",      AnchorPoint::TOP_RIGHT },
        { "left_top",       AnchorPoint::TOP_LEFT },
        { "center_bottom",  AnchorPoint::BOTTOM_CENTER },
        { "right_bottom",   AnchorPoint::BOTTOM_RIGHT },
        { "left_bottom",    AnchorPoint::BOTTOM_LEFT },
        ///@}
    }; // unordered_map stringToAnchorPoint_

    /**
     * @struct Bounds
     * @brief Represents a rectangle's position and size in 2D space.
     * @details
     * Stores the coordinates of the left, top, right, and bottom edges.
     * Provides utility methods for calculating width and height.
     * Useful for layout, collision, and rendering calculations.
     *
     * @var Bounds::left
     *   Left edge coordinate.
     * @var Bounds::top
     *   Top edge coordinate.
     * @var Bounds::right
     *   Right edge coordinate.
     * @var Bounds::bottom
     *   Bottom edge coordinate.
     */
    struct Bounds
    {
        float left = 0.0f;   ///< Left edge coordinate
        float top = 0.0f;    ///< Top edge coordinate
        float right = 0.0f;  ///< Right edge coordinate
        float bottom = 0.0f; ///< Bottom edge coordinate

        /**
         * @brief Calculates the width of the bounds.
         * @return Absolute width (right - left).
         */
        float width() const { return abs(right - left); }

        /**
         * @brief Calculates the height of the bounds.
         * @return Absolute height (bottom - top).
         */
        float height() const { return abs(bottom - top); }
        
        // Equality operator with epsilon for floating-point comparison
        bool operator==(const Bounds& other) const {
            constexpr float epsilon = 0.0001f;
            return std::abs(left - other.left)   < epsilon &&
                std::abs(top - other.top)     < epsilon &&
                std::abs(right - other.right) < epsilon &&
                std::abs(bottom - other.bottom) < epsilon;
        }

        bool operator!=(const Bounds& other) const {
            return !(*this == other);
        }
    };

} // namespace SDOM

/**
 * @brief Throws an SDOM exception with file and line info.
 * @param message Error message to report.
 */
// Macro for throwing exceptions
#define ERROR(message) throw SDOM::Exception(message, __FILE__, __LINE__)


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



