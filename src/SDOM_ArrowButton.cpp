// ============================================================================
// SDOM_ArrowButton.cpp
// ----------------------------------------------------------------------------
// Implementation file for SDOM::ArrowButton
//
// Purpose:
//     Implements the core logic, rendering, and event handling for the
//     ArrowButton class. This file defines how arrow buttons respond to
//     directional input, visual states, and Lua configuration.
//
// Notes:
//     - Keep helper functions in anonymous namespaces or mark them `static`.
//     - Avoid Doxygen comments here; documentation belongs in headers.
//     - Maintain include order: standard library, SDL, SDOM core, then local.
//     - All logging and test hooks should use SDOM-provided utilities.
//
// ----------------------------------------------------------------------------
// License: ZLIB
// ----------------------------------------------------------------------------
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
// ----------------------------------------------------------------------------
// Author: Jay Faries (https://github.com/warte67)
// ============================================================================


#include <SDOM/SDOM.hpp>
#include <SDOM/SDOM_Factory.hpp>
#include <SDOM/SDOM_SpriteSheet.hpp>
#include <SDOM/SDOM_IButtonObject.hpp>
#include <SDOM/SDOM_IconButton.hpp>
#include <SDOM/SDOM_ArrowButton.hpp>

namespace SDOM
{
    // --- Constructors --- //

    ArrowButton::ArrowButton(const InitStruct& init) : IconButton(init)
    {
        if (init.type != TypeName) {
            DEBUG_LOG("WARNING: ArrowButton constructed with incorrect type: " + init.type);
        }
        // Initialize ArrowButton-specific members
        direction_ = init.direction;
        
    } // END: ArrowButton::ArrowButton(const InitStruct& init)


    // --- Virtual Methods --- //

    bool ArrowButton::onInit() 
    {
        return SUPER::onInit(); // Call (IconButton) base class init
    } // END: bool ArrowButton::onInit() 


    void ArrowButton::onQuit() 
    {
        SUPER::onQuit(); // Call (IconButton) base class quit
    } // END: void ArrowButton::onQuit() 


    void ArrowButton::onEvent(const Event& event) 
    {
        SUPER::onEvent(event); // Call (IconButton) base class event handler first

        if (event.getType() == EventType::MouseButtonDown)  { arrowState_ = ArrowState::Depressed; }
        if (event.getType() == EventType::MouseButtonUp)    { arrowState_ = ArrowState::Raised; }

    } // END: void ArrowButton::onEvent(const Event& event) 


    void ArrowButton::onUpdate(float fElapsedTime) 
    {
        icon_index_ = getIconIndex();   // Update icon index based on current arrow state
        SUPER::onUpdate(fElapsedTime);  // Call (IconButton) base class update

    } // END: void ArrowButton::onUpdate(float fElapsedTime) 


    void ArrowButton::onRender() 
    {
        SUPER::onRender(); // Call (IconButton) base class render
    } // END: void ArrowButton::onRender() 


    bool ArrowButton::onUnitTest(int frame)
    {
        // Run base class tests first
        if (!SUPER::onUnitTest(frame))
            return false;

        UnitTests& ut = UnitTests::getInstance();
        const std::string objName = getName();

        // Only register once
        static bool registered = false;
        if (!registered)
        {
            // Validate direction enum
            ut.add_test(objName, "ArrowDirection Validity", [this](std::vector<std::string>& errors)
            {
                switch (direction_)
                {
                    case ArrowDirection::Up:
                    case ArrowDirection::Down:
                    case ArrowDirection::Left:
                    case ArrowDirection::Right:
                        break;
                    default:
                        errors.push_back(
                            "[ArrowButton] '" + getName() + 
                            "' has invalid direction: " + 
                            std::to_string(static_cast<int>(direction_))
                        );
                        break;
                }
                return true; // ✅ finished this frame
            });

            // Validate sprite sheet dimensions (if applicable)
            ut.add_test(objName, "SpriteSheet Dimensions", [this](std::vector<std::string>& errors)
            {
                SpriteSheet* ss = getSpriteSheetPtr();
                if (!ss)
                    return true; // ✅ nothing to validate if no spritesheet

                if (ss->getSpriteWidth() <= 0 || ss->getSpriteHeight() <= 0)
                {
                    errors.push_back(
                        "[ArrowButton] '" + getName() +
                        "' has invalid sprite size: w=" +
                        std::to_string(ss->getSpriteWidth()) +
                        " h=" + std::to_string(ss->getSpriteHeight())
                    );
                }
                return true; // ✅ finished this frame
            });

            registered = true;
        }

        // ✅ Return false to indicate that this test group remains active until complete
        return false;
    } // END: ArrowButton::onUnitTest()


    void ArrowButton::registerBindingsImpl(const std::string& typeName)
    {
        SUPER::registerBindingsImpl(typeName);
        BIND_INFO(typeName, "ArrowButton");
        // addFunction(typeName, "doStuff", [this]() { return this->doStuff(); });
    }    

} // END: namespace SDOM
