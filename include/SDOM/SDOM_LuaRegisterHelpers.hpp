// Helpers to register per-type Lua bindings into the C++ registry and Lua per-type table
#pragma once

#include <SDOM/SDOM_LuaRegistry.hpp>
#include <SDOM/SDOM_DisplayHandle.hpp>
#include <SDOM/SDOM_Core.hpp>
#include <sol/sol.hpp>

namespace SDOM {

template<typename F>
inline void register_per_type(sol::state_view lua, const std::string& type, const std::string& name, F&& f) {
    // Create a Lua-callable function and a protected_function wrapper from the callable,
    // then store it in the C++ registry. Some sol2 versions don't expose `make_function`,
    // so create a unique temporary global, assign the callable there and read it back as
    // a sol::function, then erase the temporary global.
    sol::protected_function pf;
    try {
        // Always create the protected_function on the canonical Core lua state to ensure
        // the callable's lifetime and userdata conversions match the runtime.
        sol::state& core = SDOM::Core::getInstance().getLua();
        std::string tmp_name = std::string("__sdom_tmp_fn_") + std::to_string(reinterpret_cast<uintptr_t>(&tmp_name));
        core[tmp_name] = std::forward<F>(f);
        sol::object obj = core[tmp_name];
        if (obj.is<sol::function>()) {
            sol::function lua_fn = obj.as<sol::function>();
            pf = sol::protected_function(lua_fn);
        } else if (obj.is<sol::protected_function>()) {
            pf = obj.as<sol::protected_function>();
        } else {
            // fallback: try converting via function constructor
            sol::function lua_fn = obj;
            pf = sol::protected_function(lua_fn);
        }
        core[tmp_name] = sol::nil;
    } catch (...) {
        // leave pf empty on error
    }
    if (pf.valid()) {
        if (type == "Label" && (name == "getFontSize" || name == "setFontSize")) {
            try { std::cout << "[TRACE_REG] register_per_type: pf.valid for " << type << "." << name << " lua_state=" << (void*)pf.lua_state() << std::endl; } catch(...) {}
        }
        // Special-case verbose print for Group.getFontSize to trace registration timing
        if (type == "Group" && name == "getFontSize") {
            try { std::cout << "[TRACE_REG] About to register Group.getFontSize on lua_state=" << (void*)lua.lua_state() << std::endl; } catch(...) {}
        }
        if (type == "Label" && name == "getFontSize") {
            try { std::cout << "[TRACE_REG] About to register Label.getFontSize on lua_state=" << (void*)lua.lua_state() << std::endl; } catch(...) {}
        }
        getLuaBindingRegistry().register_fn(type, name, pf);
        if (type == "Group" && name == "getFontSize") {
            try { std::cout << "[TRACE_REG] Registered Group.getFontSize (registry now has it?)=" << (getLuaBindingRegistry().has(type, name)?"Y":"N") << " registry_lua_state=" << (void*)getLuaBindingRegistry().get(type, name).lua_state() << std::endl; } catch(...) {}
        }
        if (type == "Label" && name == "getFontSize") {
            try { std::cout << "[TRACE_REG] Registered Label.getFontSize (registry now has it?)=" << (getLuaBindingRegistry().has(type, name)?"Y":"N") << " registry_lua_state=" << (void*)getLuaBindingRegistry().get(type, name).lua_state() << std::endl; } catch(...) {}
        }
        if (DEBUG_REGISTER_LUA) {
            try {
                std::cout << "[DEBUG] register_per_type: registered C++ registry entry for " << type << "." << name << std::endl;
            } catch(...) {}
        }
    }

    // Sanity check: warn if we're registering against a different lua_State
    try {
        void* reg_ls = (void*)lua.lua_state();
        void* core_ls = (void*)SDOM::Core::getInstance().getLua().lua_state();
        if (reg_ls != core_ls) {
            try { std::cout << "[WARN] register_per_type: registering on non-canonical lua state: reg=" << reg_ls << " core=" << core_ls << " " << type << "." << name << std::endl; } catch(...) {}
        }
    } catch(...) {}

    // Also write into the existing per-type Lua table for backwards compatibility
    try {
        // Only write into the per-type Lua table on the canonical state. If the caller
        // provided a non-canonical `lua`, skip writing to avoid cross-state protected_function issues.
        sol::state& core = SDOM::Core::getInstance().getLua();
        if ((void*)lua.lua_state() == (void*)core.lua_state()) {
            sol::table per = DisplayHandle::ensure_type_bind_table(lua, type);
            if (per.valid()) {
                // raw_set to avoid triggering metamethods
                per.raw_set(name, pf);
                try { std::cout << "[TRACE_REG] register_per_type: wrote '" << name << "' into SDOM_Bindings[" << type << "] lua_state=" << (void*)lua.lua_state() << std::endl; } catch(...) {}
                if (DEBUG_REGISTER_LUA) {
                    try {
                        std::cout << "[DEBUG] register_per_type: wrote '" << name << "' into SDOM_Bindings[" << type << "]" << std::endl;
                    } catch(...) {}
                }
            }
        } else {
            if (DEBUG_REGISTER_LUA) {
                try { std::cout << "[DEBUG] register_per_type: skipping per-type Lua table write for non-canonical lua state for " << type << "." << std::endl; } catch(...) {}
            }
        }
    } catch(...) {}
}

} // namespace SDOM
