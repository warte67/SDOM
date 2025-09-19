/**
 * @file SDOM.hpp
 * @brief Common include for SDOM-based projects. Defines macros and utilities.
 * @details
 *    ___ ___   ___  __  __   _              
 *   / __|   \ / _ \|  \/  | | |_  _ __ _ __ 
 *   \__ \ |) | (_) | |\/| |_| ' \| '_ \ '_ \
 *   |___/___/ \___/|_|  |_(_)_||_| .__/ .__/
 *                               |_|  |_|   
 *
 * The SDOM.hpp header file serves as a centralized location for 
 * defining commonly used types, global macros, and debugging 
 * utilities within the SDOM namespace. It includes type aliases, 
 * such as ID, and provides macros for error handling (ERROR), 
 * warnings (WARNING), and debugging (DEBUG) with formatted output. 
 * This file helps standardize and simplify code across the project.
 *
 * @copyright Copyright (c) 2025 Jay Faries (warte67)
 * @license ZLIB License
 * @author Jay Faries (https://github.com/warte67/SDOM)
 * @date 2025-09-19
 *
 * @note
 *   This software is provided 'as-is', without any express or implied
 *   warranty.  In no event will the authors be held liable for any damages
 *   arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *   including commercial applications, and to alter it and redistribute it
 *   freely, subject to the following restrictions:
 *   1. The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software
 *      in a product, an acknowledgment in the product documentation would be
 *      appreciated but is not required.
 *   2. Altered source versions must be plainly marked as such, and must not be
 *      misrepresented as being the original software.
 *   3. This notice may not be removed or altered from any source distribution.
 */


#ifndef __SDOM_HPP__
#define __SDOM_HPP__

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

#include "SDOM2/json.hpp" // Include nlohmann/json.hpp    
// #include "SDOM2/SDOM_SDL_Utils.hpp"
#include "SDOM2/SDOM_CLR.hpp"
// #include "SDOM2/SDOM_Core.hpp"

namespace SDOM 
{
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

    // static void quit();
    // static void shutdown();

    // static const Factory* getFactory() { return Core::instance().getFactory(); }
    // static const Core* getCore() { return &Core::instance(); }
    // static const Stage* getStage() { return Core::instance().getStage().get(); }
    // static const SDL_Renderer* getRenderer() { return Core::instance().getRenderer(); }
    // static const SDL_Window* getWindow() { return Core::instance().getWindow(); }
    // static const SDL_Texture* getTexture() { return Core::instance().getTexture(); }

} // namespace SDOM

// Macro for throwing exceptions
#define ERROR(message) throw SDOM::Exception(message, __FILE__, __LINE__)
#define WARNING(message) SDOM::Core::showWarning(message, __FILE__, __LINE__)

#define DEBUG_LOG(message) std::cout << CLR::GREEN << "    " << message << CLR::RESET << std::endl;

    // #define DEBUG_LOG(message) std::cout << CLR::GREEN << \
    //     "\n      Debug: " << \
    //     CLR::YELLOW << message << std::endl << CLR::GREEN << \
    //     "      File:  " << CLR::ORANGE << __FILE__  << CLR::GREEN << std::endl << \
    //     "      Line:  " << CLR::ORANGE  << __LINE__ << CLR::RESET << "\n" << std::endl;

#define DEBUG_ERR(message) std::cout << CLR::DARK << \
    "\n      Debug: " << \
    CLR::YELLOW << message << std::endl << CLR::DARK << \
    "      File:  " << CLR::ORANGE << __FILE__  << CLR::DARK << std::endl << \
    "      Line:  " << CLR::ORANGE  << __LINE__ << CLR::RESET << "\n" << std::endl;

#define ASSERT_EQ(val, expected) \
    if ((val) != (expected)) { \
        ERROR("Assertion failed: " + std::string(#val) + " == " + std::string(#expected) \
                  + ", actual: " + std::to_string((val)) + ", expected: " + std::to_string((expected))); \
    }


#endif // __SDOM_HPP__


