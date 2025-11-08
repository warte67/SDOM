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

    ArrowButton::ArrowButton(const sol::table& config) : IconButton(config, ArrowButton::InitStruct())
    {
        // Validate type
        std::string type = config.get_or("type", std::string(TypeName));
        if (type != TypeName) {
            DEBUG_LOG("WARNING: ArrowButton constructed with incorrect type: " + type);
        }
        
        // Initialize ArrowButton-specific members from Lua table
        if (config["direction"].valid()) {
            try {
                if (config["direction"].get_type() == sol::type::string) {
                    std::string dirStr = config["direction"].get<std::string>();
                    // normalize to lowercase
                    std::transform(dirStr.begin(), dirStr.end(), dirStr.begin(), ::tolower);
                    auto it = string_to_arrow_direction.find(dirStr);
                    direction_ = (it != string_to_arrow_direction.end()) ? it->second : ArrowDirection::Up;
                } else if (config["direction"].get_type() == sol::type::number) {
                    int dirInt = config["direction"].get<int>();
                    direction_ = (dirInt >= 0 && dirInt < 4) ? static_cast<ArrowDirection>(dirInt) : ArrowDirection::Up;
                }
            } catch (...) {
                direction_ = ArrowDirection::Up;
            }
        } else {
            direction_ = ArrowDirection::Up;
        }   
             
    } // END: ArrowButton::ArrowButton(const sol::table& config)



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




    // --- Lua Registration --- //

    void ArrowButton::_registerLuaBindings(const std::string& typeName, sol::state_view lua)
    {
        // ----------------------------------------------------------
        // 1️⃣ Base Class Registration
        // ----------------------------------------------------------
        SUPER::_registerLuaBindings(typeName, lua);

        if (DEBUG_REGISTER_LUA)
        {
            std::cout << CLR::CYAN << "[Lua] Registering type: "
                    << CLR::LT_CYAN << typeName
                    << CLR::CYAN << " (ArrowButton)" << CLR::RESET << std::endl;
        }

        // ----------------------------------------------------------
        // 2️⃣ Ensure per-type registry only (no globals, no usertypes)
        // ----------------------------------------------------------
        sol::table bindingsRoot;
        try { bindingsRoot = lua["SDOM_Bindings"]; } catch(...) {}
        if (!bindingsRoot.valid()) { bindingsRoot = lua.create_table(); lua["SDOM_Bindings"] = bindingsRoot; }
        sol::table typeTbl = bindingsRoot[typeName];
        if (!typeTbl.valid()) { typeTbl = lua.create_table(); bindingsRoot[typeName] = typeTbl; }

        // Helper: bind into SDOM_Bindings[typeName] only
        auto bind_type = [&](const char* name, auto&& func)
        {
            sol::object cur = typeTbl.raw_get_or(name, sol::lua_nil);
            if (!cur.valid() || cur == sol::lua_nil) {
                typeTbl.set_function(name, func);
            }
        };

        // ----------------------------------------------------------
        // 4️⃣ Register ArrowButton-specific Bindings
        // ----------------------------------------------------------

        bind_type("getDirection",
            [](DisplayHandle& self)
            {
                ArrowButton* btn = dynamic_cast<ArrowButton*>(self.get());
                if (!btn) return std::string("up");
                auto dir = btn->getDirection();
                auto it = ArrowButton::arrow_direction_to_string.find(dir);
                return (it != ArrowButton::arrow_direction_to_string.end()) ? it->second : "up";
            });

        bind_type("setDirection",
            [](DisplayHandle& self, const std::string& dir)
            {
                ArrowButton* btn = dynamic_cast<ArrowButton*>(self.get());
                if (!btn) return;
                auto it = ArrowButton::string_to_arrow_direction.find(dir);
                if (it != ArrowButton::string_to_arrow_direction.end())
                    btn->setDirection(it->second);
            });

        bind_type("getArrowState",
            [](DisplayHandle& self)
            {
                ArrowButton* btn = dynamic_cast<ArrowButton*>(self.get());
                if (!btn) return std::string("raised");
                auto st = btn->getArrowState();
                return (st == ArrowButton::ArrowState::Depressed) ? std::string("depressed") : std::string("raised");
            });

        bind_type("getIconIndex",
            [](DisplayHandle& self)
            {
                ArrowButton* btn = dynamic_cast<ArrowButton*>(self.get());
                return btn ? static_cast<int>(btn->getIconIndex()) : -1;
            });

        // ----------------------------------------------------------
        // 5️⃣ Done
        // ----------------------------------------------------------
        if (DEBUG_REGISTER_LUA)
        {
            std::cout << CLR::GREEN << "↳ ArrowButton Lua bindings complete."
                    << CLR::RESET << std::endl;
        }
    } // END: void IconButton::_registerLuaBindings(const std::string& typeName, sol::state_view lua)


} // END: namespace SDOM
