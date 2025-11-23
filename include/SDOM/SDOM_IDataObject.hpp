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
#include <SDOM/SDOM_DataRegistry.hpp>
#include <algorithm>
#include <cctype>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <type_traits>

// ----------------------------------------------------------------------------
// Binding log configuration and required headers
// Move these outside of the SDOM namespace to avoid including STL headers
// while a user-defined namespace is open (which would nest std in SDOM).
// ----------------------------------------------------------------------------
#ifndef SDOM_VERBOSE_BINDING_LOG
#   define SDOM_VERBOSE_BINDING_LOG true
#endif

#if SDOM_VERBOSE_BINDING_LOG
#   include <iostream>
#   include <sstream>
#endif

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
    // SDOM_VERBOSE_BINDING_LOG (defined above this namespace)
    //      When true, registerBindingsImpl() and other binding-related operations
    //      will emit debug/trace text to the console.
    //
    // Example usage:
    //      #define SDOM_VERBOSE_BINDING_LOG true
    //      #include <SDOM/SDOM_IDataObject.hpp>
    // ============================================================================


    // ============================================================================
    // ⚙️ Binding Debug Logging
    // ----------------------------------------------------------------------------
    // Enables vibrant per-type logging for the Binding / Registry system.
    // Controlled entirely by SDOM_VERBOSE_BINDING_LOG.
    // Disable in production builds to avoid console noise.
    // ============================================================================

    #if SDOM_VERBOSE_BINDING_LOG
        // Headers are included above the namespace. Only macros live here.

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
        bool onUnitTest(int frame) override { (void)frame; return true; }

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
            SDOM::FunctionInfo meta;
            meta.name = name;
            meta.cpp_signature = name + "(...)";
            registry().registerFunction(typeName, meta, std::forward<Fn>(fn));
        }

        template<typename Fn>
        void addFunction(const std::string& typeName, SDOM::FunctionInfo info, Fn&& fn)
        {
            if (info.name.empty()) {
                BIND_WARN("[" << typeName << "] addFunction called with empty name; skipping registration");
                return;
            }

            if (info.cpp_signature.empty()) {
                info.cpp_signature = info.name + "(...)";
            }
            if (info.c_name.empty()) {
                info.c_name = "SDOM_CFN_" + typeName + "_" + info.name;
            }

            BIND_LOG("[" << typeName << "] addFunction: " << info.name);
            registry().registerFunction(typeName, info, std::forward<Fn>(fn));
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
            SDOM::PropertyInfo meta;
            meta.name = name;
            meta.cpp_type = "unknown";
            registry().registerProperty(typeName, meta, std::forward<Getter>(getter), std::forward<Setter>(setter));
        }

        /**
         * @brief Register a full TypeInfo with the active registry (or global singleton).
         * @param typeName The logical type name to register under (also stored in TypeInfo.name if empty).
         * @param info The TypeInfo metadata to register.
         */
        void addDataType(const std::string& typeName, const SDOM::TypeInfo& info)
        {
            BIND_LOG("[" << typeName << "] addDataType: " << info.name);
            SDOM::TypeInfo copy = info;
            if (copy.name.empty()) copy.name = typeName;
            registry().registerDataType(copy);
        }

        /**
         * @brief Convenience for registering EventType-related TypeInfo entries.
         * @details Historically called "addEventType" in older branches; kept
         *          for convenience and backwards compatibility.
         */
        void addEventType(const SDOM::TypeInfo& info)
        {
            const std::string tn = info.name.empty() ? std::string("EventType") : info.name;
            addDataType(tn, info);
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
            // If there is an active registry set by the caller, use it
            if (s_active_registry_) {
                // preserve existing behavior while using the provided registry
                registerBindingsImpl(typeName);
                s_registered_types_.insert(typeName);
                return;
            }
            registerBindingsImpl(typeName);
            s_registered_types_.insert(typeName);
        }

        // New overload: caller supplies an explicit registry (Factory will use this)
        void registerBindings(const std::string& typeName, DataRegistry& registry)
        {
            if (s_registered_types_.count(typeName)) return;
            // set thread-local active registry for duration
            s_active_registry_ = &registry;
            registerBindingsImpl(typeName);
            s_registered_types_.insert(typeName);
            s_active_registry_ = nullptr;
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


    protected:
        SDOM::DataRegistry& registry() const
        {
            if (auto* active = IDataObject::activeRegistry()) {
                return *active;
            }
            return SDOM::DataRegistry::instance();
        }

        const SDOM::TypeInfo* lookup(const std::string& name) const
        {
            return registry().lookupType(name);
        }

        SDOM::TypeInfo& ensureType(const std::string& typeName,
                                   SDOM::EntryKind kind,
                                   std::string cppType,
                                   std::string fileStem,
                                   std::string exportName,
                                   std::string category,
                                   std::string doc)
        {
            if (lookup(typeName)) {
                return registry().getMutable(typeName);
            }

            SDOM::TypeInfo ti;
            ti.name        = typeName;
            ti.kind        = kind;
            ti.cpp_type_id = std::move(cppType);
            ti.file_stem   = std::move(fileStem);
            ti.export_name = std::move(exportName);
            ti.category    = std::move(category);
            ti.doc         = std::move(doc);

            registry().registerType(ti);
            return registry().getMutable(typeName);
        }

        void registerOpaqueHandle(const std::string& cppType,
                                  const std::string& exportName,
                                  const std::string& fileStem,
                                  const std::string& doc)
        {
            if (lookup(exportName)) {
                return;
            }

            SDOM::TypeInfo ti;
            ti.name        = exportName;
            ti.kind        = SDOM::EntryKind::Struct;
            ti.cpp_type_id = cppType;
            ti.file_stem   = fileStem;
            ti.export_name = exportName;
            ti.doc         = doc;

            SDOM::PropertyInfo implField;
            implField.name      = "impl";
            implField.cpp_type  = cppType + "*";
            implField.read_only = true;
            implField.doc       = "Opaque pointer to underlying C++ instance.";
            ti.properties.push_back(std::move(implField));

            registry().registerType(ti);
        }

        template<typename Fn>
        void registerMethod(const std::string& typeName,
                            const std::string& name,
                            const std::string& cppSig,
                            const std::string& returnType,
                            std::string cName,
                            const std::string& cSig,
                            const std::string& doc,
                            Fn&& fn)
        {
            if (name.empty()) {
                BIND_WARN("[" << typeName << "] registerMethod called with empty name; skipping registration");
                return;
            }

            if (cName.empty()) {
                cName = "SDOM_CFN_" + typeName + "_" + name;
            }

            SDOM::FunctionInfo fi;
            fi.name          = name;
            fi.cpp_signature = cppSig;
            fi.return_type   = returnType;
            fi.c_name        = std::move(cName);
            fi.c_signature   = cSig;
            fi.doc           = doc;

            registry().registerFunction(typeName, fi, std::forward<Fn>(fn));
        }

        void registerEnumValue(const std::string& exportName,
                               const std::string& fileStem,
                               const std::string& cppName,
                               std::uint32_t value,
                               const std::string& doc,
                               const std::string& category = std::string(),
                               const std::function<void(SDOM::TypeInfo&)>& customize = {})
        {
            const std::string typeName = exportName + std::string("::") + cppName;
            if (lookup(typeName)) {
                return;
            }

            SDOM::TypeInfo ti;
            ti.name        = typeName;
            ti.kind        = SDOM::EntryKind::Enum;
            ti.cpp_type_id = cppName;
            ti.file_stem   = fileStem;
            ti.export_name = exportName;
            ti.enum_value  = value;
            ti.doc         = doc;
            ti.category    = category;

            if (customize) {
                customize(ti);
            }

            registry().registerType(ti);
        }

        std::string name_ = "IDataObject";

        inline static std::unordered_set<std::string> s_registered_types_;

        // thread-local pointer to the registry used during registerBindings()
        inline static thread_local DataRegistry* s_active_registry_ = nullptr;

        static DataRegistry* activeRegistry() { return s_active_registry_; }

        // ---- Bindings ----
        virtual void registerBindingsImpl(const std::string& typeName)
        {
            BIND_INFO(typeName, "IDataObject");
        }

    };

} // namespace SDOM

#endif // __SDOM_IDataObject_HPP__
