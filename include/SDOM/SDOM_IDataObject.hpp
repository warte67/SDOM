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

#include <SDOM/SDOM.hpp>
// #define SOL_ALL_AUTOMAGIC 0
// #include <sol/sol.hpp>
#include <SDOM/SDOM_IUnitTest.hpp>


/*  Saved a note for the DisplayObject type

+    // Return a non-owning raw pointer to the requested concrete type.
+    // Example: auto* box = displayObj.as<Box>();
+    // Returned pointer is non-owning and must not be used after the underlying object is destroyed.
+    template<typename T>
+    T* as() const
+    {
+        if (!handle_.isValid()) return nullptr;
+        return dynamic_cast<T*>(handle_.get());
+    }
+
+    // Convenience: get raw IDisplayObject pointer (non-owning), or nullptr if not available.
+    IDisplayObject* get() const
+    {
+        if (!handle_.isValid()) return nullptr;
+        return dynamic_cast<IDisplayObject*>(handle_.get());
+    }

Terminology (canonical)

- DOM Node (the node / actual object)
  - The concrete object instance owned by the Factory.
  - Implements IDisplayObject (virtual interface) and contains the real state and behavior.
  - Access from C++ via DisplayObject::get() which returns a non‑owned raw pointer (IDisplayObject*).

- DisplayObject (the handle)
  - A handle/holder to a DOM Node (internally owned by the Factory).
  - References the lifetime of the DOM Node via Factory ownership semantics.
  - Provides safe accessors, property APIs and forwards mutators to the underlying DOM Node.
  - This is the canonical C++ handle type name we keep.

- AssetObject (asset handle)
  - A handle/holder to a non‑visual resource (texture, sound, data blob) managed by the Factory.
  - Use AssetHandle (alias) as the handle type exposed to C++ and Lua where appropriate.

- Resource (umbrella)
  - Generic term encompassing both DisplayObjects (visual) and AssetObjects (non-visual).

Guidelines / invariants

- Be explicit about ownership: DisplayObject == handle (not the owned DOM Node); 
    DOM Node == Factory-owned object.
- C++ APIs that need raw-pointer performance should use .get() with lifetime disclaimers.
- Register a single centralized Lua usertype (DisplayObject) exposing the handle API; 
    do not expose raw DOM Node internals to Lua.
- Add helper accessors on the handle as needed rather than exposing implementation details.

Notes on migration
- DomHandle and ResHandle are deprecated and will be removed shortly. Add short-term type aliases 
    if needed for compatibility, but prefer DisplayObject / AssetHandle in all new code and docs.


*/

namespace SDOM
{

    class IDataObject : public SDOM::IUnitTest
    {
    public:
        using Getter = std::function<sol::object(const IDataObject&, sol::state_view)>;
        using Setter = std::function<IDataObject&(IDataObject&, sol::object, sol::state_view)>;
        using Command = std::function<void(IDataObject&, sol::object, sol::state_view)>;
        using Function = std::function<sol::object(IDataObject&, sol::object, sol::state_view)>;

        virtual bool onInit() = 0;
        virtual void onQuit() = 0;
        virtual bool onUnitTest() { return true; }

        // void fromLua(const sol::table& lua, sol::state_view lua_state);
        // sol::table toLua(sol::state_view lua) const;

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
        
        void registerLua(const std::string& typeName, sol::state_view lua)      // Depricated
        { }                                  // Depricated

        // New preferred Lua binding path
        virtual void registerDisplayObject(const std::string& typeName, sol::state_view lua)
        {
            this->_registerDisplayObject(typeName, lua);
        }

    protected:

        virtual void _registerDisplayObject(const std::string& typeName, sol::state_view lua)
        {
            if (DEBUG_REGISTER_LUA)
            {
                std::string typeNameLocal = "IDataObject";
                std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal 
                        << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN << typeName << CLR::RESET << std::endl;
            }
        }   


        
        sol::usertype<IDataObject> objHandleType_;

        // --- End New Virtual LUA Registration for Sol2 ---


    protected:

        std::unordered_map<std::string, Getter> getters_;
        std::unordered_map<std::string, Setter> setters_;
        std::unordered_map<std::string, Command> commands_;

        std::string name_ = "IDataObject";  // Default name, should be overridden by derived classes

    };

} // namespace SDOM

#endif // __SDOM_IDataObject_HPP__