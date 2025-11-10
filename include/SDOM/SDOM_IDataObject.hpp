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
#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <type_traits>

// Ensure DEBUG_REGISTER_LUA exists (normally provided by SDOM.hpp). If the
// umbrella header is included by a TU it will override this; this definition
// acts as a safe default when headers are included individually.
// #ifndef DEBUG_REGISTER_LUA
// // #define DEBUG_REGISTER_LUA 0
// constexpr bool DEBUG_REGISTER_LUA = true;
// #endif


namespace SDOM
{
    // ============================================================================
    // ⚙️  Configuration Flags
    // ----------------------------------------------------------------------------
    // SDOM_VERBOSE_BINDING_LOG
    //      When true, registerBindingsImpl() and other binding-related operations
    //      will emit debug/trace text to the console.
    //
    // Example usage:
    //      #define SDOM_VERBOSE_BINDING_LOG true
    //      #include <SDOM/SDOM_IDataObject.hpp>
    // ============================================================================
    #ifndef SDOM_VERBOSE_BINDING_LOG
        #define SDOM_VERBOSE_BINDING_LOG true
    #endif


    // ============================================================================
    // ⚙️ Binding Debug Logging
    // ----------------------------------------------------------------------------
    // Enables vibrant per-type logging for the Binding / Registry system.
    // Controlled entirely by SDOM_VERBOSE_BINDING_LOG.
    // Disable in production builds to avoid console noise.
    // ============================================================================

    #if SDOM_VERBOSE_BINDING_LOG
        #include <iostream>
        #include <sstream>
        #include <SDOM/SDOM_CLR.hpp>

        // 💬 Normal Binding Trace
        #define BIND_LOG(...) do {                                       \
            std::ostringstream _sdom_bind_oss;                           \
            _sdom_bind_oss << __VA_ARGS__;                               \
            std::cout << CLR::LT_MAGENTA                                 \
                    << "[BIND]" << CLR::MAGENTA << " "                   \
                    << _sdom_bind_oss.str() << CLR::RESET << std::endl;  \
        } while(0)

        // ⚠️ Warning (e.g. missing field, duplicate entry)
        #define BIND_WARN(...) do {                                      \
            std::ostringstream _sdom_bind_warn;                          \
            _sdom_bind_warn << __VA_ARGS__;                              \
            std::cout << CLR::LT_YELLOW                                  \
                    << "[BIND WARN]" << CLR::YELLOW << " "               \
                    << _sdom_bind_warn.str() << CLR::RESET << std::endl; \
        } while(0)

        // ❌ Error (binding failure, exception, abort)
        #define BIND_ERR(...) do {                                       \
            std::ostringstream _sdom_bind_err;                           \
            _sdom_bind_err << __VA_ARGS__;                               \
            std::cerr << CLR::LT_RED                                     \
                    << "[BIND ERROR]" << CLR::RESET << " "               \
                    << _sdom_bind_err.str() << CLR::RESET << std::endl;  \
        } while(0)

    #else
        #define BIND_LOG(...)  do { } while(0)
        #define BIND_WARN(...) do { } while(0)
        #define BIND_ERR(...)  do { } while(0)
    #endif


    // ============================================================================
    // IDataObject
    // -----------------------------------------------------------------------------
    // The root interface for all data-driven objects in SDOM. It inherits from
    // both IUnitTest and IDataBindingBase to provide runtime binding, reflection,
    // and serialization behavior.
    // ============================================================================
    class IDataObject : public SDOM::IUnitTest
    {
        
    public:
        // ---- Lifecycle ----
        virtual bool onInit() = 0;
        virtual void onQuit() = 0;
        virtual bool onUnitTest(int frame) override { (void)frame; return true; }

        // ---- Identity ----
        std::string getName() const { return name_; }
        void setName(const std::string& newName) { name_ = newName; }

        // ============================================================
        // 🧱 Placeholders for upcoming Data Registry integration
        // ============================================================

        /**
         * @brief Placeholder for adding a function to the registry.
         * @param typeName The object type being registered.
         * @param name The exposed function name.
         * @param fn The callable function or lambda.
         */
        template<typename Fn>
        void addFunction(const std::string& typeName, const std::string& name, Fn&& fn)
        {
            BIND_LOG("[" << typeName << "] addFunction: " << name);
            // Placeholder: store or register function later with DataRegistry.
            // e.g. DataRegistry::getInstance().registerFunction(typeName, name, fn);
        }

        /**
         * @brief Placeholder for adding a property to the registry.
         * @param typeName The object type being registered.
         * @param name The exposed property name.
         * @param getter Getter function pointer or lambda.
         * @param setter Setter function pointer or lambda (optional).
         */
        template<typename Getter, typename Setter = std::nullptr_t>
        void addProperty(const std::string& typeName, const std::string& name, Getter&& getter, Setter&& setter = nullptr)
        {
            BIND_LOG("[" << typeName << "] addProperty: " << name);
            // Placeholder: will connect to DataRegistry later.
        }

        // =============================================================
        // 🧩 Public Binding Entry Point
        // =============================================================
        /**
         * @brief Public entrypoint to trigger this object's binding registration.
         * @details This is the polymorphic root. Derived classes may safely call
         *          `registerBindings()` without knowing the concrete type.
         */
        void registerBindings(const std::string& typeName)
        {
            // BIND_LOG("IDataObject::registerBindings for " << typeName);
            // BIND_INFO(typeName, "IDataObject");

            // Guard to ensure we don’t double-register the same type
            if (s_registered_types_.count(typeName)) {
                // BIND_WARN("Type already registered: " << typeName);
                return;
            }
            registerBindingsImpl(typeName);
            s_registered_types_.insert(typeName);
        }

        void BIND_INFO(const std::string& typeName, const std::string& typeNameLocal)
        {
            if (SDOM_VERBOSE_BINDING_LOG)
            {
                std::cout << CLR::fg_rgb(128, 255, 255) << "[" << typeName << "]"
                        << CLR::fg_rgb(64, 192, 192) << " Registering bindings for type: "
                         << CLR::fg_rgb(128, 255, 255) << typeNameLocal 
                         << CLR::RESET << std::endl;
            }
        }




        // ======================================================================
        // Legacy Lua Binding Helpers
        // ======================================================================

    public:
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

        static sol::table ensure_sol_table(sol::state_view lua, const std::string& typeName)
        {
            sol::table global = lua.globals();
            sol::object maybe = global.raw_get_or(typeName, sol::lua_nil);
            if (maybe.valid() && maybe != sol::lua_nil)
            {
                // Many sol2 builds expose a usertype as a table/metatable; some
                // report it as a userdata. In either case, fetch the table view
                // from the global entry without replacing it.
                try {
                    if (maybe.is<sol::table>()) {
                        return maybe.as<sol::table>();
                    }
                } catch(...) {}
                // Fallback: treat the existing global as a table-like and return it
                try { return lua[typeName]; } catch(...) {}
            }
            // Not present: create and publish a fresh table
            sol::table newTable = lua.create_table();
            global[typeName] = newTable;
            if (false)
            {
                std::cout << CLR::MAGENTA << "[LUA] Created table for type: "
                        << CLR::LT_MAGENTA << typeName << CLR::RESET << std::endl;
            }
            return newTable;
        }           

        // Register a usertype and attach it to the given Lua table (idempotent)
        template <typename T, typename BaseT>
        static sol::usertype<T> register_usertype_with_table(sol::state_view lua, const std::string& typeName)
        {
            // Guard against re-registration across multiple call sites and
            // avoid clobbering existing usertype tables. We only track a
            // boolean flag per (lua_State*, typeName), not sol objects.
            using StateMap = std::unordered_map<std::string, bool>;
            static std::unordered_map<lua_State*, StateMap> s_registered;
            lua_State* L = lua.lua_state();
            auto& reg = s_registered[L];
            auto it = reg.find(typeName);
            if (it != reg.end() && it->second) {
                // Already registered in this state; return a handle to the existing usertype table.
                try { return lua[typeName]; } catch(...) {
                    // If the table is somehow missing, fall through to create below.
                }
            }

            // If a table already exists and was marked as registered, reuse it.
            sol::object existing = lua.globals().raw_get_or(typeName, sol::lua_nil);
            if (existing.valid() && existing != sol::lua_nil) {
                try {
                    sol::table t = lua[typeName];
                    sol::object flag = t.raw_get_or("__usertype_registered", sol::lua_nil);
                    if (flag.valid() && flag.is<bool>() && flag.as<bool>()) {
                        reg[typeName] = true;
                        return lua[typeName];
                    }
                } catch(...) {}
            }

            // Create once (no constructor exposed to Lua by default).
            auto userType = lua.new_usertype<T>(
                typeName,
                sol::no_constructor,
                sol::base_classes, sol::bases<BaseT>()
            );

            // Mark on the usertype table itself so subsequent calls can detect it.
            try {
                sol::table utbl = lua[typeName];
                utbl["__usertype_registered"] = true;
            } catch(...) {}

            if (false)
            {
                std::cout << CLR::YELLOW << "[LUA] Registered usertype: "
                        << CLR::LT_YELLOW << typeName << CLR::RESET << std::endl;
            }

            reg[typeName] = true;
            return userType;
        }        

        // Return both legacy per-type table (globals[typeName]) and canonical
        // registry table (SDOM_Bindings[typeName]). Also ensures a proper
        // usertype is registered with the provided BaseT.
        template <typename T, typename BaseT>
        static std::pair<sol::table, sol::table> ensure_type_tables(sol::state_view lua, const std::string& typeName)
        {
            // Ensure usertype with base relationship (idempotent)
            (void)register_usertype_with_table<T, BaseT>(lua, typeName);

            // Legacy/compatibility per-type table in globals
            sol::table legacy = ensure_sol_table(lua, typeName);

            // Canonical registry root: SDOM_Bindings
            sol::table root;
            try { root = lua["SDOM_Bindings"]; } catch(...) {}
            if (!root.valid()) {
                root = lua.create_table();
                lua["SDOM_Bindings"] = root;
            }

            // Canonical per-type registry: SDOM_Bindings[typeName]
            sol::table reg = root.raw_get_or(typeName, sol::lua_nil);
            if (!reg.valid()) {
                reg = lua.create_table();
                root[typeName] = reg;
            }

            return { legacy, reg };
        }

        // Convenience: ensure usertype + tables, and return the canonical
        // registry table (SDOM_Bindings[typeName]).
        template <typename T, typename BaseT>
        static sol::table getTypeTable(sol::state_view lua, const std::string& typeName)
        {
            auto both = ensure_type_tables<T, BaseT>(lua, typeName);
            return both.second; // canonical registry table
        }

        // New preferred Lua binding path
        void registerLuaBindings(const std::string& typeName, sol::state_view lua)
        {
            this->_registerLuaBindings(typeName, lua);
        }        


    protected:
        std::string name_ = "IDataObject";

        inline static std::unordered_set<std::string> s_registered_types_;

        // ---- Bindings ----
        virtual void registerBindingsImpl(const std::string& typeName)
        {
            BIND_INFO(typeName, "IDataObject");
        }










        //  ============================================================
        // 🧱 Legacy Lua Binding Helpers
        //  ============================================================
        virtual void _registerLuaBindings(const std::string& typeName, sol::state_view lua)
        {
            // 1. Make sure the per-type Lua table exists.
            sol::table typeTable;
            try {
                typeTable = ensure_sol_table(lua, typeName);
            } catch (...) {
                return;  // If Lua isn’t ready yet, fail silently.
            }

            // 2. Make sure we run this block only once per type.
            sol::object registered = typeTable.raw_get_or("__idataobject_initialized", sol::lua_nil);
            if (registered.valid() && registered.is<bool>() && registered.as<bool>()) {
                return; // Already initialized; ancestors can still add keys later.
            }
            typeTable["__idataobject_initialized"] = true;

            // 3. Add the base-level helpers that every IDataObject should expose.
            auto set_if_absent = [](sol::table& tbl, const char* name, auto&& fn) {
                sol::object cur = tbl.raw_get_or(name, sol::lua_nil);
                if (!cur.valid() || cur == sol::lua_nil) {
                    tbl.set_function(name, std::forward<decltype(fn)>(fn));
                }
            };

            set_if_absent(typeTable, "getName", [](IDataObject& self) { return self.getName(); });
            set_if_absent(typeTable, "setName", [](IDataObject& self, const std::string& newName) { self.setName(newName); });

            // if (DEBUG_REGISTER_LUA)
            if (false)
            {
                std::string typeNameLocal = "IDataObject";
                std::cout << CLR::CYAN << "Registered " << CLR::LT_CYAN << typeNameLocal
                        << CLR::CYAN << " Lua bindings for type: " << CLR::LT_CYAN
                        << typeName << CLR::RESET << std::endl;
            }            
        }   
        
        sol::usertype<IDataObject> objHandleType_;

        // --- End New Virtual LUA Registration for Sol2 ---

    };

} // namespace SDOM

#endif // __SDOM_IDataObject_HPP__
