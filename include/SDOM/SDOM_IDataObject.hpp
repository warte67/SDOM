/***  SDOM_IDataObject.hpp  ****************************
 *  ___ ___   ___  __  __   ___ ___       _         ___  _     _        _     _              
 * / __|   \ / _ \|  \/  | |_ _|   \ __ _| |_ __ _ / _ \| |__ (_)___ __| |_  | |_  _ __ _ __ 
 * \__ \ |) | (_) | |\/| |  | || |) / _` |  _/ _` | (_) | '_ \| / -_) _|  _|_| ' \| '_ \ '_ \
 * |___/___/ \___/|_|  |_|_|___|___/\__,_|\__\__,_|\___/|_.__// \___\__|\__(_)_||_| .__/ .__/
 *                      |___|                               |__/                  |_|  |_|   
 *  
 * The SDOM_IDataObject class defines the core interface for all data-driven objects within 
 * the SDOM framework. It provides a flexible property and command registration system, 
 * enabling dynamic serialization, deserialization, and runtime manipulation of object state 
 * through JSON. By supporting property getters, setters, and commands, IDataObject allows for 
 * introspection, scripting, and editor integration, making it easy to extend and interact with 
 * objects in a generic way. This interface forms the foundation for resource and display object 
 * management in SDOM, facilitating robust data binding, configuration, and automation across 
 * the entire framework.
 * 
 *   This software is provided 'as-is', without any express or implied
 *   warranty.  In no event will the authors be held liable for any damages
 *   arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *   including commercial applications, and to alter it and redistribute it
 *   freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not
 *       claim that you wrote the original software. If you use this software
 *       in a product, an acknowledgment in the product documentation would be
 *       appreciated but is not required.
 *   2. Altered source versions must be plainly marked as such, and must not be
 *       misrepresented as being the original software.
 *   3. This notice may not be removed or altered from any source distribution.
 *
 * Released under the ZLIB License.
 * Original Author: Jay Faries (warte67)
 *
 ******************/

/*
================================================================================
SDOM::IDataObject Contract

Intent:
    - IDataObject is the core interface for all data-driven objects in SDOM.
    - Provides a flexible, dynamic property and command system for runtime manipulation, introspection, and automation.
    - Enables serialization and deserialization of object state for scripting, configuration, and editor integration.

Requirements:
    1. Dynamic Properties
        - Objects can register named properties with getter/setter functions.
        - Properties are accessible for reading and writing at runtime.
        - Properties can be introspected (listed, queried) for editor and scripting use.

    2. Dynamic Commands
        - Objects can register named commands (methods) callable at runtime.
        - Commands can accept arguments and perform actions on the object.
        - Commands are introspectable for automation and scripting.

    3. Serialization/Deserialization
        - Objects can serialize their state to a data format (now Lua tables).
        - Objects can be initialized/deserialized from a data format (Lua tables).
        - Serialization includes all registered properties and children (for hierarchical objects).

    4. Editor/Scripting Integration
        - Properties and commands are discoverable for external tools (editors, consoles, scripts).
        - Supports runtime modification and automation.

    5. Extensibility
        - New properties and commands can be registered at runtime.
        - Supports inheritance and composition for complex objects.

Non-Requirements:
    - Not responsible for rendering, event handling, or resource management (handled by other interfaces).
    - Not tied to a specific serialization format (was JSON, now Lua; could be extended).
`
Summary:
    IDataObject is a data-centric, introspectable, and extensible interface for SDOM objects, focused on:
        - Dynamic property/command registration
        - Runtime introspection and manipulation
        - Serialization/deserialization for scripting, configuration, and editor integration
================================================================================
*/


#ifndef __SDOM_IDataObject_HPP__
#define __SDOM_IDataObject_HPP__

// Avoid including the heavy umbrella SDOM.hpp here to prevent include cycles.
// We still need small utilities used by this header: CLR (for colored prints)

#include <sol/sol.hpp>
#include <SDOM/SDOM_CLR.hpp>
#include <SDOM/SDOM_IUnitTest.hpp>

// Ensure DEBUG_REGISTER_LUA exists (normally provided by SDOM.hpp). If the
// umbrella header is included by a TU it will override this; this definition
// acts as a safe default when headers are included individually.
// #ifndef DEBUG_REGISTER_LUA
// // #define DEBUG_REGISTER_LUA 0
// constexpr bool DEBUG_REGISTER_LUA = true;
// #endif


namespace SDOM
{

    class IDataObject : public SDOM::IUnitTest
    {
        
    public:

        virtual bool onInit() = 0;
        virtual void onQuit() = 0;
        virtual bool onUnitTest() override { return true; }

        template<typename T>
        static T lua_value_case_insensitive(const sol::table& tbl, const std::string& key, const T& default_value)
        {
            for (const auto& kv : tbl)
            {
                std::string k = kv.first.as<std::string>();
                if (std::equal(k.begin(), k.end(), key.begin(), key.end(),
                    [](char a, char b) { return std::tolower(a) == std::tolower(b); }))
                {
                    return kv.second.as<T>();
                }
            }
            return default_value;
        }

        std::string getName() const { return name_; }
        void setName(const std::string& newName) { name_ = newName; }

        // --- New Virtual LUA Registration for Sol2 ---
    public:

        // New preferred Lua binding path
        void registerLuaBindings(const std::string& typeName, sol::state_view lua)
        {
            this->_registerLuaBindings(typeName, lua);
        }

    protected:

        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua)
        {
            // if (DEBUG_REGISTER_LUA)
            if (false) // TEMP DISABLE
            {
                std::string typeNameLocal = "IDataObject";
                std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                        << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN << typeName << CLR::RESET << std::endl;
            }
        }   
        
        sol::usertype<IDataObject> objHandleType_;

        // --- End New Virtual LUA Registration for Sol2 ---

    protected:

        std::string name_ = "IDataObject";  // Default name, should be overridden by derived classes

    };

} // namespace SDOM

#endif // __SDOM_IDataObject_HPP__