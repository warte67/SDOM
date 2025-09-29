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
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
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

constexpr bool DEBUG_REGISTER_LUA = false;



/**
 * @namespace SDOM
 * @brief Contains all core classes and utilities for the SDOM library.
 */
namespace SDOM 
{
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
        float left;   ///< Left edge coordinate
        float top;    ///< Top edge coordinate
        float right;  ///< Right edge coordinate
        float bottom; ///< Bottom edge coordinate

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
#define DEBUG_LOG(message) do { std::cout << CLR::GREEN << "[DEBUG] " << (message) << CLR::RESET << std::endl; } while(0)

/**
 * @brief Logs an informational message to stdout.
 * @param message Informational message to display.
 */
#define INFO(message) std::cout << CLR::YELLOW << "[INFO] " << message << CLR::RESET << std::endl

/**
 * @brief Logs a debug error (non-fatal) with file and line info in dark text.
// Macro for informational logging (statement-safe, parenthesized parameter)
#define INFO(message) do { std::cout << CLR::YELLOW << "[INFO] " << (message) << CLR::RESET << std::endl; } while(0)
// Macro for debug errors (non-fatal)
#define DEBUG_ERR(message) std::cout << CLR::DARK << \
    "\n      Debug: " << \
    CLR::YELLOW << message << std::endl << CLR::DARK << \
    "      File:  " << CLR::ORANGE << __FILE__  << CLR::DARK << std::endl << \
    "      Line:  " << CLR::ORANGE  << __LINE__ << CLR::RESET << "\n" << std::endl;

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

// #include <SDOM/SDOM_Handle.hpp>
#include <SDOM/SDOM_DomHandle.hpp>
#include <SDOM/SDOM_ResHandle.hpp>

#include <SDOM/SDOM_UnitTests.hpp>
#include <SDOM/SDOM_Utils.hpp>

#ifndef SDOM_USE_INDIVIDUAL_HEADERS
    // SDOM_Front.hpp is an umbrella header meant for implementation files only.
    // Including it from SDOM.hpp can create circular includes (SDOM.hpp -> SDOM_Front.hpp -> SDOM_Core.hpp -> SDOM.hpp)
    // which may leave class definitions incomplete during header processing. Consumers who want the full
    // API should include SDOM_Front.hpp directly in .cpp files. Keep this block intentionally empty here
    // to avoid accidental circular include dependencies.
#endif




